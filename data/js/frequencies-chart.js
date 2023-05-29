// Crear el contexto del gráfico de frecuencias
var freqsCtx = document.getElementById('freqs-chart').getContext('2d');
var freqsChart = new Chart(freqsCtx, {
    type: 'bar',
    data: {
        labels: freqsHz, // Etiquetas de frecuencias
        datasets: [{
            label: 'Amplitud de los armonicos',
            data: freqsAmplitude, // Datos de frecuencias
            backgroundColor: 'rgba(0, 255, 0, 0.5)',
            borderColor: 'rgba(0, 255, 0, 1)',
            borderWidth: 1
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
                    text: 'Frecuencias [Hz]'
                }
            },
            y: {
                display: true,
                title: {
                    display: true,
                    text: 'Amplitud'
                },
                suggestedMin: 0,
                suggestedMax: 100
            }
        }
    }
});