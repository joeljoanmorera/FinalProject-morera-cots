// Datos de ejemplo del cardiograma
var datosCardiograma = [0, 1, 2, 3, 4, 3, 2, 1, 0, -1, -2, -3, -4, -3, -2, -1, 0];
    
var canvas = document.getElementById('cardiograma');
var ctx = canvas.getContext('2d');

// Establecer el tamaño del lienzo
canvas.width = datosCardiograma.length * 10;
canvas.height = 200;

// Dibujar el cardiograma
ctx.beginPath();
ctx.moveTo(0, canvas.height / 2);

for (var i = 0; i < datosCardiograma.length; i++) {
  ctx.lineTo(i * 10, (canvas.height / 2) - datosCardiograma[i] * 10);
}

ctx.strokeStyle = 'red';
ctx.lineWidth = 2;
ctx.stroke();