// Constants
const maxDataLength = 32;
// Arrays
let heartRateArray = [];
let spo2Array = [];
let timeArray = [];
let rawTimes = [];
let freqsHz = [];
let freqsAmplitude = [];

// Last values
let lastFreqsHz = [];
let lastFreqsAmplitude = [];

// Values vars
let bpm = "Calculating BPM...";
let spo = "Calculating SPO2...";

// Conectar al WebSocket del ESP32
var socket = new WebSocket('ws://' + location.hostname + '/ws');
socket.onmessage = function (event) {
    // get new data
    var jsonData = JSON.parse(event.data);

    // update bpm and spo2
    var beatsPerMinute = jsonData.beatsPerMinute;
    bpm = beatsPerMinute + " BPM";
    document.getElementById('heartrate').innerHTML = bpm;
    
    var spo2Percentage = jsonData.spo2Percentage;
    spo = spo2Percentage + " %";
    document.getElementById('spo2').innerHTML = spo;

    // update heartRateArray, spo2Array and timeArray
    var newHeartRateData = jsonData.heartRateData;
    heartRateArray.push(newHeartRateData);
    if (heartRateArray.length >= maxDataLength) heartRateArray.shift();

    var newspo2Data = jsonData.spo2Data;
    spo2Array.push(newspo2Data);
    if (spo2Array.length >= maxDataLength) spo2Array.shift();
    
    if(rawTimes.length != 0)
    {
        var actualTime = Date.now();

        var millisTimeDifference = actualTime - rawTimes[rawTimes.length - 1];
        var timeDifferenceBetweenData = millisTimeDifference/1000;
        console.log("timeDifferenceBetweenData: " + timeDifferenceBetweenData);
        // calculate accomulated time and round it to 3 decimals
        var accomulatedTime = Math.floor((timeArray[timeArray.length - 1] + timeDifferenceBetweenData)*1000)/1000;
        console.log("accomulatedTime: " + accomulatedTime);
        
        rawTimes.push(actualTime);
        timeArray.push(accomulatedTime);
    }
    else
    {
        var initialTime = Date.now();
        rawTimes.push(initialTime);
        timeArray.push(0);
    }
    if (timeArray.length > maxDataLength) {
        rawTimes.shift();
        timeArray.shift();
    }
    cardiogramaChart.update();
    spo2Chart.update();
    
    // update freqsHz and freqsAmplitude
    var freqsDataHz = jsonData.freqsHz;
    var similarityBetweenHzArrays =  (lastFreqsHz.length == freqsDataHz.length) 
        && lastFreqsHz.every(function(element, index) {
        return element === freqsDataHz[index]; 
    });

    if (!similarityBetweenHzArrays){
        freqsHz.length = 0;
        for(var i = 0; i < freqsDataHz.length; i++)
        {
            freqsHz.push(freqsDataHz[i]);
        }
        lastFreqsHz = freqsDataHz;
    }
    
    var freqsDataAmplitude = jsonData.freqsAmplitude;
    var similarityBetweenAmplitudeArrays = (lastFreqsAmplitude.length == freqsDataAmplitude.length) 
        && lastFreqsAmplitude.every(function(element, index) {
        return element === freqsDataAmplitude[index]; 
    });
    if (!similarityBetweenAmplitudeArrays){
        freqsAmplitude.length = 0;
        for(var i = 0; i < freqsDataAmplitude.length; i++){
            freqsAmplitude.push(freqsDataAmplitude[i]);
        }
        lastFreqsAmplitude = freqsDataAmplitude;
        
        freqsChart.update();
    }
}