const express = require('express');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json());
app.use(express.static('.'));

app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Headers', 'Content-Type');
  res.header('Access-Control-Allow-Methods', 'POST, GET, OPTIONS');
  if (req.method === 'OPTIONS') return res.sendStatus(200);
  next();
});

app.post('/api/command', (req, res) => {
  const { command } = req.body;

  if (!command) {
    return res.status(400).json({ error: 'Comando requerido' });
  }

  const heviPath = path.join(__dirname, 'HeVi');

  const hevi = spawn(heviPath);

  let output = '';
  let errorOutput = '';

  hevi.stdout.on('data', (data) => {
    output += data.toString();
  });

  hevi.stderr.on('data', (data) => {
    errorOutput += data.toString();
  });

  hevi.on('close', (code) => {
    if (code !== 0 && errorOutput) {
      return res.status(500).json({ error: errorOutput });
    }
    res.json({ result: output });
  });

  hevi.on('error', (err) => {
    res.status(500).json({ error: 'Error al ejecutar HeVi: ' + err.message });
  });

  hevi.stdin.write(command + '\nSALIR\n');
  hevi.stdin.end();
});

app.listen(PORT, () => {
  console.log(`HeVi API corriendo en http://localhost:${PORT}`);
  console.log(`Abre http://localhost:${PORT}/index.html para usar la interfaz`);
});