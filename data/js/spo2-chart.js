// Crear el contexto del gráfico de SPO2
var spo2Ctx = document.getElementById('spo2-chart').getContext('2d');
var spo2Chart = new Chart(spo2Ctx, {
    type: 'line',
    data: {
        labels: timeArray, // Etiquetas de tiempo
        datasets: [{
            label: 'Saturacion de oxigeno en sangre',
            data: spo2Array, // Datos del SPO2
            backgroundColor: 'rgba(255, 0, 0, 0.5)',
            borderColor: 'rgba(255, 0, 0, 1)',
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
                display: true,
                title: {
                    display: true,
                    text: 'Tiempo'
                }
            },
            y: {
                display: true,
                title: {
                    display: true,
                    text: 'Nivel de oxígeno (%)'
                },
                suggestedMin: 0,
                suggestedMax: 100
            }
        }
    }
});