// Arrays
const heartRateArray = [0,0,0,0];
const spo2Array = [0,0,0,0];
const timeArray = [1,2,3,4];
var bpm = "0 BPM";
var spo = "0 %";

// Conectar al WebSocket del ESP32
var socket = new WebSocket('ws://' + location.hostname + '80:/ws');
socket.onmessage = function(event) {
    var raw = event.data;
    const data = raw.split(';'); // Separar los datos (tiempo, amplitud, spo2, bpm
    var tiempo = data[0]; // Obtener tiempo del dato
    var amplitud = data[1]; // Obtener amplitud del dato
    var spo2 = data[2]; // Obtener nivel de SPO2 del dato
    bpm = data[3]; // Obtener latidos por minuto
    spo = data[4]; // Obtener valor de SPO2

    // Agregar el tiempo
    timeArray.push(tiempo);

    // Agregar la amplitud al gráfico de cardiograma
    heartRateArray.push(amplitud);

    // Agregar el nivel de SPO2 al gráfico de SPO2
    spo2Array.push(spo2);

    // Limitar la cantidad de datos a mostrar a 30 segundos (600 muestras a 50 Hz)
    var maxDataLength = 600;
    if (timeArray.length > maxDataLength) {
        timeArray.shift();
    }

    if (heartRateArray.length > maxDataLength) {
        heartRateArray.shift();
    }

    if (spo2Array.length > maxDataLength) {
        spo2Array.shift();
    }

    // Actualizar los gráficos
    cardiogramaChart.update();
    spo2Chart.update();
};