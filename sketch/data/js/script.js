$(document).ready(function () {
    // defining variables
    var targetTemp = document.getElementById("chartTemp");
    var targetIndex = document.getElementById("chartIndex");
    var targetHumd = document.getElementById("chartHumd");
    var gaugeTemp, gaugeIndex, gaugeHumd;

    var relay1 = $('.relay1');
    var relay2 = $('.relay2');
    var relay3 = $('.relay3');
    var relay4 = $('.relay4');

    initGauge();

    // webSockets
    var ws = new WebSocket("ws://" + window.location.hostname + "/ws");
    // webSocket connection open
    ws.onopen = function () {
        $('#status').html("Connected");
        $('#status').css('color', '#6AA84F');
    };
    // webSocket connection close
    ws.onclose = function () {
        $('#status').html("Disconnected");
        $('#status').css('color', '#CC0200');
    };
    // webSocket data receive
    ws.onmessage = function (evt) {
        var data = evt.data;

        // if id matches then toggle relays
        if (data == "on-1") {
            relayOn(relay1);
        }
        if (data == "off-1") {
            relayOff(relay1);
        }
        if (data == "on-2") {
            relayOn(relay2);
        }
        if (data == "off-2") {
            relayOff(relay2);
        }
        if (data == "on-3") {
            relayOn(relay3);
        }
        if (data == "off-3") {
            relayOff(relay3);
        }
        if (data == "on-4") {
            relayOn(relay4);
        }
        if (data == "off-4") {
            relayOff(relay4);
        }

        if (data.includes("T: ")) {
            var temperature = data.replace("T: ", "");
            // setting gauge temperature value
            gaugeTemp.set(temperature);
            $('#temp').html(Math.round(temperature * 10) / 10);
        }
        if (data.includes("H: ")) {
            var humidity = data.replace("H: ", "");
            // setting gauge humidity value
            gaugeHumd.set(humidity);
            $('#humd').html(Math.round(humidity));
        }
        if (data.includes("I: ")) {
            var heatIndex = data.replace("I: ", "");
            // setting gauge heat index value
            gaugeIndex.set(heatIndex);
            $('#index').html(heatIndex);
        }
    };

    // sending commands to websockets
    relay1.click(function() {
        if (relay1.hasClass("relayOn")) {
            ws.send("off-1");
            relayOff(relay1);
        } else {
            ws.send("on-1");
            relayOn(relay1);
        }
    });
    relay2.click(function() {
        if (relay2.hasClass("relayOn")) {
            ws.send("off-2");
            relayOff(relay2);
        } else {
            ws.send("on-2");
            relayOn(relay2);
        }
    });
    relay3.click(function() {
        if (relay3.hasClass("relayOn")) {
            ws.send("off-3");
            relayOff(relay3);
        } else {
            ws.send("on-3");
            relayOn(relay3);
        }
    });
    relay4.click(function() {
        if (relay4.hasClass("relayOn")) {
            ws.send("off-4");
            relayOff(relay4);
        } else {
            ws.send("on-4");
            relayOn(relay4);
        }
    });

    // toggling relay classes
    function relayOn(relay) {
        relay.removeClass("relayOff");
        relay.addClass("relayOn");
        relay.html('Turn Off');
    }
    function relayOff(relay) {
        relay.removeClass("relayOn");
        relay.addClass("relayOff");
        relay.html('Turn On');
    }

    function initGauge() {
        // color values for gauge
        var colorValuesTempIndex = [
            {strokeStyle: "#6AA84F", min: 0, max: 17},  // green
            {strokeStyle: "#FF9903", min: 18, max: 32}, // orange
            {strokeStyle: "#CC0200", min: 33, max: 80} // red
        ];
        var colorValuesHumd = [
            {strokeStyle: "#6AA84F", min: 0, max: 40},  // green
            {strokeStyle: "#FF9903", min: 41, max: 60}, // orange
            {strokeStyle: "#CC0200", min: 61, max: 80} // red
        ];

        // options for gauge
        var optsTempIndex = {
            angle: 0, 
            lineWidth: 0.3, 
            radiusScale: 1, 
            pointer: {
                length: 0.6, 
                strokeWidth: 0.035, 
                color: '#000000' 
            },
            limitMax: false,     
            limitMin: false,     
            colorStart: '#6FADCF',   
            colorStop: '#8FC0DA',    
            strokeColor: '#E0E0E0',  
            generateGradient: true,
            highDpiSupport: true,
            staticZones: colorValuesTempIndex
        };
        var optsHumd = {
            angle: 0, 
            lineWidth: 0.3, 
            radiusScale: 1, 
            pointer: {
                length: 0.6, 
                strokeWidth: 0.035, 
                color: '#000000' 
            },
            limitMax: false,     
            limitMin: false,     
            colorStart: '#6FADCF',   
            colorStop: '#8FC0DA',    
            strokeColor: '#E0E0E0',  
            generateGradient: true,
            highDpiSupport: true,
            staticZones: colorValuesHumd
        };
        
        // setting gauges
        gaugeTemp = new Gauge(targetTemp).setOptions(optsTempIndex);
        gaugeIndex = new Gauge(targetIndex).setOptions(optsTempIndex);
        gaugeHumd = new Gauge(targetHumd).setOptions(optsHumd);

        // initializing gauge with values
        gaugeTemp.set(0);
        gaugeIndex.set(0);
        gaugeHumd.set(0);
    }
    
});