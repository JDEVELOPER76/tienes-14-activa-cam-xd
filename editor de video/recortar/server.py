import os
import uuid
import shutil
import subprocess
from pathlib import Path

from fastapi import FastAPI, File, UploadFile, HTTPException
from fastapi.responses import FileResponse, HTMLResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel

app = FastAPI(title="Recortador de video sin pérdida")

BASE_DIR = Path(__file__).parent
UPLOAD_DIR = BASE_DIR / "uploads"
STATIC_DIR = BASE_DIR / "static"
UPLOAD_DIR.mkdir(exist_ok=True)

app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")


class TrimRequest(BaseModel):
    filename: str
    start: float
    end: float


def _run(cmd: list[str]) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, capture_output=True, text=True)


def get_duration(filepath: Path) -> float:
    """
    Obtiene la duración del video en segundos.
    Estrategia en cascada porque algunos contenedores (WebM, MKV)
    no siempre traen la duración en el header.
    """

    # 1) Duración desde el contenedor (format)
    r = _run([
        "ffprobe", "-v", "error",
        "-show_entries", "format=duration",
        "-of", "default=noprint_wrappers=1:nokey=1",
        str(filepath),
    ])
    dur = _parse_float(r.stdout)
    if dur and dur > 0:
        return dur

    # 2) Duración del stream de video (algunos WebM la traen aquí)
    r = _run([
        "ffprobe", "-v", "error",
        "-select_streams", "v:0",
        "-show_entries", "stream=duration",
        "-of", "default=noprint_wrappers=1:nokey=1",
        str(filepath),
    ])
    dur = _parse_float(r.stdout)
    if dur and dur > 0:
        return dur

    # 3) Fallback: decodificar todo y quedarnos con el último timestamp
    #    (más lento, pero funciona con WebM sin duración en header)
    r = _run([
        "ffprobe", "-v", "error",
        "-select_streams", "v:0",
        "-show_entries", "packet=pts_time",
        "-of", "csv=p=0",
        str(filepath),
    ])
    last = None
    for line in r.stdout.splitlines():
        line = line.strip().rstrip(",")
        if not line:
            continue
        try:
            last = float(line)
        except ValueError:
            continue
    if last is not None and last > 0:
        return last

    # 4) Último recurso: usar ffmpeg para medir
    #    (lee el archivo completo, pero es lo más fiable)
    r = _run([
        "ffmpeg", "-i", str(filepath),
        "-f", "null", "-",
    ])
    # ffmpeg escribe la duración al final en stderr: "time=00:00:12.34"
    import re
    matches = re.findall(r"time=(\d+):(\d+):(\d+\.\d+)", r.stderr)
    if matches:
        h, m, s = matches[-1]
        return int(h) * 3600 + int(m) * 60 + float(s)

    return 0.0


def _parse_float(text: str) -> float:
    try:
        return float(text.strip())
    except (ValueError, AttributeError):
        return 0.0


@app.get("/", response_class=HTMLResponse)
async def index():
    return (STATIC_DIR / "index.html").read_text(encoding="utf-8")


@app.post("/upload")
async def upload(video: UploadFile = File(...)):
    ext = Path(video.filename).suffix or ".mp4"
    filename = f"{uuid.uuid4().hex}{ext}"
    filepath = UPLOAD_DIR / filename

    with filepath.open("wb") as buffer:
        shutil.copyfileobj(video.file, buffer)

    duration = get_duration(filepath)
    if duration <= 0:
        filepath.unlink(missing_ok=True)
        raise HTTPException(
            status_code=400,
            detail="No se pudo leer la duración del video. "
                   "Puede que el archivo esté corrupto o use un códec no soportado.",
        )

    return {"filename": filename, "duration": duration}


@app.post("/trim")
async def trim(req: TrimRequest):
    input_path = UPLOAD_DIR / req.filename
    if not input_path.exists():
        raise HTTPException(status_code=404, detail="Archivo no encontrado")

    duration = req.end - req.start
    if duration <= 0:
        raise HTTPException(status_code=400, detail="Rango inválido")

    output_filename = f"{input_path.stem}_trimmed{input_path.suffix}"
    output_path = UPLOAD_DIR / output_filename

    # -c copy = sin recodificar. Para WebM funciona igual (VP8/VP9/AV1 + Opus/Vorbis)
    cmd = [
        "ffmpeg", "-y",
        "-ss", str(req.start),
        "-i", str(input_path),
        "-t", str(duration),
        "-c", "copy",
        "-avoid_negative_ts", "make_zero",
        str(output_path),
    ]

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
    except subprocess.TimeoutExpired:
        raise HTTPException(status_code=500, detail="FFmpeg tardó demasiado")

    if result.returncode != 0:
        raise HTTPException(
            status_code=500,
            detail=f"FFmpeg falló: {result.stderr[-500:]}",
        )

    return {"output": output_filename}


@app.get("/download/{filename}")
async def download(filename: str):
    safe_name = Path(filename).name
    path = UPLOAD_DIR / safe_name
    if not path.exists():
        raise HTTPException(status_code=404, detail="Archivo no encontrado")

    # Detectar el media type según la extensión para que el navegador lo abra bien
    ext = path.suffix.lower()
    media_types = {
        ".webm": "video/webm",
        ".mp4":  "video/mp4",
        ".mkv":  "video/x-matroska",
        ".mov":  "video/quicktime",
        ".avi":  "video/x-msvideo",
    }
    media_type = media_types.get(ext, "application/octet-stream")

    return FileResponse(path, media_type=media_type, filename=safe_name)


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
