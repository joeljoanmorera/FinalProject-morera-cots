// Constants
const maxDataLength = 32;
// Arrays
let heartRateArray = [];
let timeArray = [];
let freqsHz = [];
let freqsAmplitude = [];

// Last values
let lastFreqsHz = [];
let lastFreqsAmplitude = [];
let lastRawTime = 0;

// Values vars
let beatsPerMinuteValue = "Calculating BPM...";
let spo2PercentageValue = "Calculating SPO2...";

// Conectar al WebSocket del ESP32
var socket = new WebSocket('ws://' + location.hostname + '/ws');
socket.onmessage = function (event) {
    // get new data
    var jsonData = JSON.parse(event.data);

    // update beats per minute and spo2
    var beatsPerMinute = jsonData.beatsPerMinute;
    beatsPerMinuteValue = beatsPerMinute + " BPM";
    document.getElementById('heartrate').innerHTML = beatsPerMinuteValue;
    
    var spo2Percentage = jsonData.spo2Percentage;
    spo2PercentageValue = spo2Percentage + " %";
    document.getElementById('spo2').innerHTML = spo2PercentageValue;

    // update heartRateArray, spo2Array and timeArray
    var newHeartRateData = jsonData.heartRateData;
    heartRateArray.push(newHeartRateData);
    if (heartRateArray.length >= maxDataLength) heartRateArray.shift();

    if(lastRawTime != 0)
    {
        var actualTime = Date.now();
        var millisTimeDifference = actualTime - lastRawTime;
        var timeDifferenceBetweenData = millisTimeDifference/1000;
        
        // calculate accomulated time and round it to 3 decimals
        var accomulatedTime = Math.floor((timeArray[timeArray.length - 1] + timeDifferenceBetweenData)*1000)/1000;        
        lastRawTime = actualTime;
        timeArray.push(accomulatedTime);
    } else {
        lastRawTime = Date.now();
        timeArray.push(0);
    }
    if ( timeArray.length > maxDataLength ) timeArray.shift();
    
    cardiogramaChart.update();
    
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