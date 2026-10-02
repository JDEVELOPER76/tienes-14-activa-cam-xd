#!/usr/bin/env python3
"""
Servidor local que sirve el editor HTML y ejecuta FFmpeg nativo.
Uso: python3 servidor.py
"""

import http.server
import socketserver
import subprocess
import tempfile
import os
import json
import shutil
import uuid
import io
from urllib.parse import urlparse

PORT = 8080
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_DIR = os.path.join(BASE_DIR, "salidas")
os.makedirs(OUTPUT_DIR, exist_ok=True)


class Handler(http.server.SimpleHTTPRequestHandler):
    """Sirve archivos estáticos y expone /unir para procesar videos."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=BASE_DIR, **kwargs)

    def do_POST(self):
        if self.path == '/unir':
            self.handle_unir()
        else:
            self.send_error(404, "Not found")

    def handle_unir(self):
        try:
            # 1. Leer multipart/form-data
            content_type = self.headers.get('Content-Type', '')
            if 'multipart/form-data' not in content_type:
                self.send_json(400, {"error": "Content-Type inválido"})
                return

            content_length = int(self.headers.get('Content-Length', 0))
            body = self.rfile.read(content_length)

            # Parsear multipart manualmente
            boundary = content_type.split('boundary=')[1].encode()
            partes = body.split(b'--' + boundary)

            videos = []       # lista de (filename, bytes) en orden
            resolucion = "1280x720"

            for parte in partes:
                if b'Content-Disposition' not in parte:
                    continue

                # Separar headers y contenido
                if b'\r\n\r\n' not in parte:
                    continue
                headers_raw, contenido = parte.split(b'\r\n\r\n', 1)
                contenido = contenido.rstrip(b'\r\n--')

                headers_txt = headers_raw.decode('utf-8', errors='ignore')
                nombre = ""
                if 'name="' in headers_txt:
                    nombre = headers_txt.split('name="')[1].split('"')[0]

                if nombre == 'resolucion':
                    resolucion = contenido.decode('utf-8').strip()
                elif nombre == 'videos[]' or nombre.startswith('videos'):
                    # Extraer filename
                    filename = "video.mp4"
                    if 'filename="' in headers_txt:
                        filename = headers_txt.split('filename="')[1].split('"')[0]
                    videos.append((filename, contenido))

            if len(videos) < 2:
                self.send_json(400, {"error": "Se necesitan al menos 2 videos"})
                return

            print(f"\n📥 Recibidos {len(videos)} videos, resolución {resolucion}")

            # 2. Guardar videos temporalmente
            temp_dir = tempfile.mkdtemp(prefix="editor_")
            try:
                rutas = []
                for i, (filename, data) in enumerate(videos):
                    ext = filename.rsplit('.', 1)[-1].lower() if '.' in filename else 'mp4'
                    ruta = os.path.join(temp_dir, f"input{i}.{ext}")
                    with open(ruta, 'wb') as f:
                        f.write(data)
                    rutas.append(ruta)
                    print(f"  [{i+1}/{len(videos)}] {filename} "
                          f"({len(data)/1024/1024:.1f} MB)")

                # 3. Unir con FFmpeg
                salida = os.path.join(OUTPUT_DIR, f"video_{uuid.uuid4().hex[:8]}.mp4")
                self.unir_videos(rutas, salida, resolucion)

                # 4. Devolver el archivo
                print(f"✅ Listo: {salida}")
                self.send_file(salida, os.path.basename(salida))

            finally:
                shutil.rmtree(temp_dir, ignore_errors=True)

        except subprocess.CalledProcessError as e:
            print(f"❌ Error FFmpeg: {e}")
            self.send_json(500, {"error": f"Error de FFmpeg: {e}"})
        except Exception as e:
            import traceback
            traceback.print_exc()
            self.send_json(500, {"error": str(e)})

    def unir_videos(self, rutas, salida, resolucion):
        """Normaliza y concatena con FFmpeg nativo."""
        w, h = resolucion.split('x')
        temp_dir = os.path.dirname(rutas[0])

        # Normalizar cada clip
        print("🔄 Normalizando clips...")
        normalizados = []
        for i, ruta in enumerate(rutas):
            out = os.path.join(temp_dir, f"norm{i}.mp4")
            print(f"  Normalizando {i+1}/{len(rutas)}...")
            subprocess.run([
                "ffmpeg", "-y", "-hide_banner", "-loglevel", "error",
                "-i", ruta,
                "-vf", f"scale={w}:{h}:force_original_aspect_ratio=decrease,"
                       f"pad={w}:{h}:(ow-iw)/2:(oh-ih)/2,setsar=1",
                "-r", "30",
                "-c:v", "libx264",
                "-preset", "veryfast",
                "-crf", "23",
                "-c:a", "aac",
                "-ar", "44100",
                "-ac", "2",
                "-pix_fmt", "yuv420p",
                out
            ], check=True)
            normalizados.append(out)

        # Concatenar
        print("🎬 Concatenando...")
        lista = os.path.join(temp_dir, "lista.txt")
        with open(lista, 'w') as f:
            for n in normalizados:
                f.write(f"file '{n}'\n")

        subprocess.run([
            "ffmpeg", "-y", "-hide_banner", "-loglevel", "error",
            "-f", "concat", "-safe", "0",
            "-i", lista,
            "-c", "copy",
            salida
        ], check=True)

    def send_json(self, code, obj):
        data = json.dumps(obj).encode('utf-8')
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def send_file(self, path, filename):
        size = os.path.getsize(path)
        self.send_response(200)
        self.send_header('Content-Type', 'video/mp4')
        self.send_header('Content-Length', str(size))
        self.send_header('Content-Disposition',
                         f'attachment; filename="{filename}"')
        self.end_headers()
        with open(path, 'rb') as f:
            shutil.copyfileobj(f, self.wfile)

    def log_message(self, format, *args):
        # Silenciar logs de archivos estáticos (menos ruido)
        if '/unir' in (args[0] if args else ''):
            super().log_message(format, *args)


if __name__ == "__main__":
    # Verificar que ffmpeg esté instalado
    if shutil.which("ffmpeg") is None:
        print("❌ FFmpeg no está instalado.")
        print("   Instálalo con: sudo apt install ffmpeg")
        exit(1)

    print(f"🎬 Editor de Video - Servidor Local")
    print(f"   → http://localhost:{PORT}")
    print(f"   → FFmpeg: {shutil.which('ffmpeg')}")
    print(f"   → Ctrl+C para detener\n")

    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n👋 Servidor detenido")
