// ============================================================
//  Editor de Video - Cliente
// ============================================================

let clips = []; // { id, file, url }

// --- DOM ---
const fileInput        = document.getElementById('fileInput');
const dropzone         = document.getElementById('dropzone');
const listaClips       = document.getElementById('listaClips');
const emptyState       = document.getElementById('emptyState');
const btnUnir          = document.getElementById('btnUnir');
const progresoContainer= document.getElementById('progresoContainer');
const barraFill        = document.getElementById('barraFill');
const estado           = document.getElementById('estado');
const porcentaje       = document.getElementById('porcentaje');
const preview          = document.getElementById('preview');
const previewEmpty     = document.getElementById('previewEmpty');
const resolucion       = document.getElementById('resolucion');

// ============================================================
//  1. Cargar archivos (input + drag&drop)
// ============================================================
fileInput.addEventListener('change', (e) => {
  agregarArchivos(Array.from(e.target.files));
  fileInput.value = '';
});

['dragenter', 'dragover'].forEach(ev =>
  dropzone.addEventListener(ev, (e) => {
    e.preventDefault();
    dropzone.classList.add('dragover');
  })
);

['dragleave', 'drop'].forEach(ev =>
  dropzone.addEventListener(ev, (e) => {
    e.preventDefault();
    dropzone.classList.remove('dragover');
  })
);

dropzone.addEventListener('drop', (e) => {
  const files = Array.from(e.dataTransfer.files)
    .filter(f => f.type.startsWith('video/'));
  agregarArchivos(files);
});

function agregarArchivos(files) {
  for (const file of files) {
    if (!file.type.startsWith('video/')) continue;
    clips.push({
      id: crypto.randomUUID(),
      file,
      url: URL.createObjectURL(file)
    });
  }
  renderLista();
}

// ============================================================
//  2. Render lista
// ============================================================
function renderLista() {
  listaClips.innerHTML = '';

  emptyState.classList.toggle('hidden', clips.length > 0);

  clips.forEach((clip, i) => {
    const li = document.createElement('li');
    li.draggable = true;
    li.dataset.id = clip.id;

    // SVG: handle (arrastrar)
    const handleSVG = `
      <svg viewBox="0 0 24 24" fill="none" stroke="currentColor"
           stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="9" cy="6" r="1"/><circle cx="9" cy="12" r="1"/>
        <circle cx="9" cy="18" r="1"/><circle cx="15" cy="6" r="1"/>
        <circle cx="15" cy="12" r="1"/><circle cx="15" cy="18" r="1"/>
      </svg>`;

    const basuraSVG = `
      <svg viewBox="0 0 24 24" fill="none" stroke="currentColor"
           stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <polyline points="3 6 5 6 21 6"/>
        <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/>
      </svg>`;

    li.innerHTML = `
      <span class="orden">${i + 1}</span>
      <span class="handle">${handleSVG}</span>
      <span class="info">
        <span class="nombre">${escapeHTML(clip.file.name)}</span>
        <span class="meta">${(clip.file.size / 1024 / 1024).toFixed(1)} MB</span>
      </span>
      <button class="btn-eliminar" title="Eliminar">${basuraSVG}</button>
    `;

    li.querySelector('.btn-eliminar').onclick = (ev) => {
      ev.stopPropagation();
      clips = clips.filter(c => c.id !== clip.id);
      renderLista();
    };

    li.addEventListener('click', () => {
      preview.src = clip.url;
      preview.play();
      previewEmpty.classList.add('hidden');
    });

    agregarDragHandlers(li);
    listaClips.appendChild(li);
  });

  btnUnir.disabled = clips.length < 2;
}

// ============================================================
//  3. Drag & drop para reordenar
// ============================================================
let dragId = null;

function agregarDragHandlers(li) {
  li.addEventListener('dragstart', () => {
    dragId = li.dataset.id;
    li.classList.add('dragging');
  });
  li.addEventListener('dragend', () => {
    li.classList.remove('dragging');
    dragId = null;
  });
  li.addEventListener('dragover', (e) => e.preventDefault());
  li.addEventListener('drop', (e) => {
    e.preventDefault();
    if (!dragId || dragId === li.dataset.id) return;
    const from = clips.findIndex(c => c.id === dragId);
    const to   = clips.findIndex(c => c.id === li.dataset.id);
    const [mov] = clips.splice(from, 1);
    clips.splice(to, 0, mov);
    renderLista();
  });
}

// ============================================================
//  4. Unir videos
// ============================================================
btnUnir.addEventListener('click', async () => {
  try {
    btnUnir.disabled = true;
    progresoContainer.hidden = false;
    setProgreso(0, 'Enviando videos al servidor...');

    const formData = new FormData();
    formData.append('resolucion', resolucion.value);
    for (const clip of clips) {
      formData.append('videos[]', clip.file, clip.file.name);
    }

    // Progreso simulado hasta que llegue la respuesta
    let fake = 0;
    const intervalo = setInterval(() => {
      fake = Math.min(fake + 1.5, 92);
      setProgreso(fake, 'Procesando con FFmpeg nativo...');
    }, 800);

    const respuesta = await fetch('/unir', {
      method: 'POST',
      body: formData
    });

    clearInterval(intervalo);

    if (!respuesta.ok) {
      const err = await respuesta.json().catch(() => ({ error: 'Error del servidor' }));
      throw new Error(err.error || `HTTP ${respuesta.status}`);
    }

    setProgreso(97, 'Descargando resultado...');
    const blob = await respuesta.blob();
    const url = URL.createObjectURL(blob);

    preview.src = url;
    previewEmpty.classList.add('hidden');

    const a = document.createElement('a');
    a.href = url;
    a.download = `video_unido_${Date.now()}.mp4`;
    document.body.appendChild(a);
    a.click();
    a.remove();

    setProgreso(100, `Listo · ${(blob.size / 1024 / 1024).toFixed(1)} MB`);
    toast('Video exportado correctamente', 'success');

    setTimeout(() => {
      progresoContainer.hidden = true;
      setProgreso(0, '');
    }, 3000);

  } catch (err) {
    console.error(err);
    setProgreso(0, '');
    progresoContainer.hidden = true;
    toast('Error: ' + err.message);
  } finally {
    btnUnir.disabled = false;
  }
});

// ============================================================
//  Utilidades
// ============================================================
function setProgreso(pct, texto) {
  barraFill.style.width = pct + '%';
  porcentaje.textContent = Math.round(pct) + '%';
  if (texto) estado.textContent = texto;
}

function escapeHTML(s) {
  return s.replace(/[&<>"']/g, c => ({
    '&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'
  }[c]));
}

function toast(msg, tipo = 'error') {
  const el = document.createElement('div');
  el.className = 'toast' + (tipo === 'success' ? ' success' : '');
  el.textContent = msg;
  document.body.appendChild(el);
  setTimeout(() => {
    el.style.opacity = '0';
    el.style.transition = 'opacity .3s';
    setTimeout(() => el.remove(), 300);
  }, 4000);
}
