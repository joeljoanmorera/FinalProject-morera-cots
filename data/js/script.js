// Arrays
const heartRateArray = [1,1,1,2,3,1];
const spo2Array = [1,1,1,2,3,1];
const timeArray = [0,1,2,3,4,5];
var bpm = "0 BPM";
var spo = "0 %";

// Conectar al WebSocket del ESP32
var socket = new WebSocket('ws://' + location.hostname + '80:/ws');
// socket.onmessage = function(event) {
//     var raw = event.data;
//     const data = raw.split(';'); // Separar los datos (tiempo, amplitud, spo2, bpm
//     var tiempo = data[0]; // Obtener tiempo del dato
//     var amplitud = data[1]; // Obtener amplitud del dato
//     var spo2 = data[2]; // Obtener nivel de SPO2 del dato
//     bpm = data[3]; // Obtener latidos por minuto
//     spo = data[4]; // Obtener valor de SPO2

//     // Agregar el tiempo
//     timeArray.push(tiempo);

//     // Agregar la amplitud al gráfico de cardiograma
//     heartRateArray.push(amplitud);

//     // Agregar el nivel de SPO2 al gráfico de SPO2
//     spo2Array.push(spo2);

//     // Limitar la cantidad de datos a mostrar a 30 segundos (600 muestras a 50 Hz)
//     var maxDataLength = 600;
//     if (timeArray.length > maxDataLength) {
//         timeArray.shift();
//     }

//     if (heartRateArray.length > maxDataLength) {
//         heartRateArray.shift();
//     }

//     if (spo2Array.length > maxDataLength) {
//         spo2Array.shift();
//     }

//     // Actualizar los gráficos
//     cardiogramaChart.update();
//     spo2Chart.update();
// };
socket.onmessage = function(event) {
    var raw = event.data;
    var jsonData = JSON.parse(raw);

    // Extraer los datos del objeto JSON
    var heartRateDataArray = jsonData.heartRateDataArray;
    var spo2DataArray = jsonData.spo2DataArray;
    var beatsPerMinute = jsonData.beatsPerMinute;
    var spo2Percentage = jsonData.spo2Percentage;

    // Agregar los datos a las variables correspondientes
    for (var i = 0;i < heartRateDataArray.lenght; i++){
        timeArray.push(i);
    }
    for (var i = 0; i < heartRateDataArray.length; i++) {
        heartRateArray.push(heartRateDataArray[i]);
    }

    for (var i = 0; i < spo2DataArray.length; i++) {
        spo2Array.push(spo2DataArray[i]);
    }

    bpm = beatsPerMinute;
    spo = spo2Percentage;

    // Limitar la cantidad de datos a mostrar a 30 segundos (600 muestras a 50 Hz)
    var maxDataLength = 600;
    if (heartRateArray.length > maxDataLength) {
        heartRateArray.splice(0, heartRateArray.length - maxDataLength);
    }

    if (spo2Array.length > maxDataLength) {
        spo2Array.splice(0, spo2Array.length - maxDataLength);
    }

    // Actualizar los gráficos
    cardiogramaChart.update();
    spo2Chart.update();
};