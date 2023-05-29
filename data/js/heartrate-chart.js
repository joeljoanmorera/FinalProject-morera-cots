// Crear el contexto del gráfico de cardiograma
var cardiogramaCtx = document.getElementById('cardiograma-chart').getContext('2d');
var cardiogramaChart = new Chart(cardiogramaCtx, {
    type: 'line',
    data: {
        labels: timeArray, // Etiquetas de tiempo
        datasets: [{
            label:'Amplitud en mV',
            data: heartRateArray, // Datos del cardiograma
            backgroundColor: 'rgba(0, 123, 255, 0.5)',
            borderColor: 'rgba(0, 123, 255, 1)',
            borderWidth: 1,
            fill: false,
            lineTension: 0
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: {
                display: false,
                title: {
                    display: true,
                    text: 'Tiempo [s]'
                }
            },
            y: {
                display: true,
                title: {
                    display: true,
                    text: 'Amplitud [mV]'
                }
            }
        }
    }
});