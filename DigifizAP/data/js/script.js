$(document).ready(function () {
    
    const PARAMETER_RPMCOEFFICIENT=0;
    const PARAMETER_SPEEDCOEEFICIENT=1;
    const PARAMETER_COOLANTTHERMISTORB=2;
    const PARAMETER_OILTHERMISTORB=3;
    const PARAMETER_AIRTHERMISTORB=4;
    const PARAMETER_TANKMINRESISTANCE=5;
    const PARAMETER_TANKMAXRESISTANCE=6;
    const PARAMETER_TAU_COOLANT=7;
    const PARAMETER_TAU_OIL=8;
    const PARAMETER_TAU_AIR=9;
    const PARAMETER_TAU_TANK=10;
    const PARAMETER_MILEAGE=11;
    const PARAMETER_DAILY_MILEAGE=12;
    const PARAMETER_AUTO_BRIGHTNESS=13;
    const PARAMETER_BRIGHTNESS_LEVEL=14;
    const PARAMETER_TANK_CAPACITY=15;
    const PARAMETER_MFA_STATE=16;
    const PARAMETER_BUZZER_OFF=17;
    const PARAMETER_MAX_RPM=18;
    const PARAMETER_DOT_OFF=23;
    const PARAMETER_BACKLIGHT_ON=24;
    const PARAMETER_M_D_FILTER=25;
    const PARAMETER_COOLANT_MAX_R=26;
    const PARAMETER_COOLANT_MIN_R=27;
    const PARAMETER_COMMAND_MFA_RESET=28;
    const PARAMETER_COMMAND_MFA_MODE=29;
    const PARAMETER_COMMAND_MFA_BLOCK=30;
    const PARAMETER_READ_ADDITION=128;
    const PARAMETER_SET_HOUR=255;
    const PARAMETER_SET_MINUTE=254;
    const PARAMETER_RESET_DAILY_MILEAGE=253;
    const PARAMETER_RESET_DIGIFIZ=252;

    //Data acquisition
    const PARAMETER_GET_ACCUMULATED_UPTIME=251;
    const PARAMETER_GET_COOLANT_TEMPERATURE=250;
    const PARAMETER_GET_OIL_TEMPERATURE=249;
    const PARAMETER_GET_AMBIENT_TEMPERATURE=248;
    const PARAMETER_GET_FUEL_IN_TANK=247;
    const PARAMETER_GET_SPEED=246;
    const PARAMETER_GET_RPM=245;
    const PARAMETER_SET_DAY=244;
    const PARAMETER_SET_MONTH=243;
    const PARAMETER_SET_YEAR=242;
    const PARAMETER_GET_DAY=241;
    const PARAMETER_GET_MONTH=240;
    const PARAMETER_GET_YEAR=239;
    const PARAMETER_GET_HOUR=238;
    const PARAMETER_GET_MINUTE=237;

    // defining variables
    var targetSpeed = document.getElementById("chartSpeed");
    var targetRPM = document.getElementById("chartRPM");
    var targetTemp = document.getElementById("chartTemp");
    var targetOil = document.getElementById("chartOil");
    var targetAir = document.getElementById("chartAir");
    var targetFuel = document.getElementById("chartFuel");
    
    var gaugeSpeed, gaugeRPM, gaugeTemp, gaugeOil, gaugeAir, gaugeFuel;

    var mfablock = $('.mfablock');
    var mfareset = $('.mfareset');
    var mfamode = $('.mfamode');
    var bckAutoBtn = $('.bckAutoBtn');
    var bckManBtn = $('.bckManBtn');
    var setTimeBtn = $('.setTimeBtn');
    var setTCapBtn = $('.setTCapBtn');
    var sendCmdBtn = $('.sendCmdBtn');
    var setBckBtn = $('.setBckBtn');
    var getDataBtn = $('.getDataBtn');
    var realtimeBtn = $('.realtimeBtn');
    var startLogBtn = $('.startLogBtn');
    var stopLogBtn = $('.stopLogBtn');
    var getLogBtn = $('.getLogBtn');
    var factResetBtn = $('.factResetBtn');
    
    var realtimeQueryActive = 0;
    var curParam = 237;
    
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
        console.log(data);
        if (data.includes(PARAMETER_GET_ACCUMULATED_UPTIME+": ")) {
            var vData = data.replace(PARAMETER_GET_ACCUMULATED_UPTIME+": ", "");
            console.log(vData);
        }
        else if (data.includes(PARAMETER_GET_COOLANT_TEMPERATURE+": ")) {
            var vData = data.replace(PARAMETER_GET_COOLANT_TEMPERATURE+": ", "");
            console.log(vData);
            // setting gauge temperature value
            gaugeTemp.set(Math.round(vData) / 120.0*80.0);
            $('#coolantT').html(Math.round(vData));
        }
        else if (data.includes(PARAMETER_GET_OIL_TEMPERATURE+": ")) {
            var vData = data.replace(PARAMETER_GET_OIL_TEMPERATURE+": ", "");
            console.log(vData);
            // setting gauge temperature value
            gaugeOil.set(Math.round(vData) / 120.0*80.0);
            $('#oilT').html(Math.round(vData));
        }
        else if (data.includes(PARAMETER_GET_AMBIENT_TEMPERATURE+": ")) {
            var vData = data.replace(PARAMETER_GET_AMBIENT_TEMPERATURE+": ", "");
            console.log(vData);
            // setting gauge temperature value
            gaugeAir.set((Math.round(vData) +50.0)/ 100.0*80.0);
            $('#airT').html(Math.round(vData));
        }
        else if (data.includes(PARAMETER_GET_FUEL_IN_TANK+": ")) {
            var vData = data.replace(PARAMETER_GET_FUEL_IN_TANK+": ", "");
            console.log(vData);
            // setting gauge temperature value
            gaugeFuel.set(Math.round(vData)/60.0*80.0);
            $('#fuel').html(Math.round(vData * 10) / 10);
        }
         else if (data.includes(PARAMETER_GET_SPEED+": ")) {
            var vData = data.replace(PARAMETER_GET_SPEED+": ", "");
            console.log(vData);
            // setting gauge temperature value
            gaugeSpeed.set(Math.round(vData) / 250.0*80.0);
            $('#speed').html(Math.round(vData));
        }
        else if (data.includes(PARAMETER_GET_RPM+": ")) {
            var vData = data.replace(PARAMETER_GET_RPM+": ", "");
            console.log(vData);
            // setting gauge temperature value
            gaugeRPM.set(Math.round(vData)/8000.0*80.0);
            $('#rpm').html(Math.round(vData));
        }
        else if (data.includes(PARAMETER_GET_DAY+": ")) {
            var vData = data.replace(PARAMETER_GET_DAY+": ", "");
            console.log(vData);
        }
        else if (data.includes(PARAMETER_GET_MONTH+": ")) {
            var vData = data.replace(PARAMETER_GET_MONTH+": ", "");
            console.log(vData);
        }
        else if (data.includes(PARAMETER_GET_DAY+": ")) {
            var vData = data.replace(PARAMETER_GET_DAY+": ", "");
            console.log(vData);
        }
        else if (data.includes(PARAMETER_GET_YEAR+": ")) {
            var vData = data.replace(PARAMETER_GET_YEAR+": ", "");
            console.log(vData);
        }
        else if (data.includes(PARAMETER_GET_HOUR+": ")) {
            var vData = data.replace(PARAMETER_GET_HOUR+": ", "");
            document.getElementById("hour").value = vData;
            console.log(vData);
        }
        else if (data.includes(PARAMETER_GET_MINUTE+": ")) {
            var vData = data.replace(PARAMETER_GET_MINUTE+": ", "");
            document.getElementById("minute").value = vData;
            console.log(vData);
        }
    };

    // sending commands to websockets
    
    var intervalID = setInterval(
        function()
        {
            console.log("realtime update");
            if (realtimeQueryActive)
            {
                //237..251
                //getDataBtn.click();
                ws.send(curParam+" 0");
                curParam+=1;
                if (curParam==252)
                    curParam=237;
            }
        }, 500);
    
    mfablock.click(function() {
        ws.send("mfablock");
    });

    mfareset.click(function() {
        ws.send("mfareset");
    });
    
    mfamode.click(function() {
        ws.send("mfamode");
    });
    
    bckAutoBtn.click(function() {
        ws.send("bckAutoBtn");
    });
    
    bckManBtn.click(function() {
        ws.send("bckManBtn");
    });
    
    setTimeBtn.click(function() {
        var hourStr = $("#hour").val();
        var minute = $("#minute").val();
        ws.send("255 "+hourStr);
        ws.send("254 "+minute);
    });
    
    sendCmdBtn.click(function() {
        var cmd = $("#cmd").val();
        var parameter = $("#parameter").val();
        ws.send(cmd+" "+parameter);
        //ws.send("sendCmdBtn");
    });
    
    setBckBtn.click(function() {
        var bl_level = $("#bl_level").val();
        ws.send("14 "+bl_level);
    });
    
    getDataBtn.click(function() {
        ws.send("getDataBtn");
    });
    
    sendCmdBtn.click(function() {
        ws.send("sendCmdBtn");
    });
    
    realtimeBtn.click(function() {
        //ws.send("realtimeBtn");
        if (realtimeQueryActive==0)
        {
            document.getElementById("realtimeBtn").innerHTML = "Query active";
            realtimeQueryActive = 1;
        }
        else
        {
            document.getElementById("realtimeBtn").innerHTML = "Realtime";
            realtimeQueryActive = 0;
        }
    });
    
    startLogBtn.click(function() {
        ws.send("startLogBtn");
    });
    
    stopLogBtn.click(function() {
        ws.send("stopLogBtn");
    });
    setTCapBtn.click(function() {
        var tcap = $("#tcap").val();
        ws.send("15 "+tcap);
    });
    
    factResetBtn.click(function() {
        ws.send("252 0");
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
        var colorValuesSpeed = [
            {strokeStyle: "#6AA84F", min: 0, max: 17},  // green
            {strokeStyle: "#FF9903", min: 18, max: 32}, // orange
            {strokeStyle: "#CC0200", min: 33, max: 80} // red
        ];
        
        var colorValuesRPM = [
            {strokeStyle: "#6AA84F", min: 0, max: 60},  // green
            {strokeStyle: "#FF9903", min: 61, max: 69}, // orange
            {strokeStyle: "#CC0200", min: 70, max: 80} // red
        ];
        
        var colorValuesTemp = [
            {strokeStyle: "#6AA84F", min: 0, max: 40},  // green
            {strokeStyle: "#FF9903", min: 41, max: 60}, // orange
            {strokeStyle: "#CC0200", min: 61, max: 80} // red
        ];
        
        var colorValuesOil = [
            {strokeStyle: "#6AA84F", min: 0, max: 40},  // green
            {strokeStyle: "#FF9903", min: 41, max: 60}, // orange
            {strokeStyle: "#CC0200", min: 61, max: 80} // red
        ];
        
        var colorValuesAir = [
            {strokeStyle: "#6AA84F", min: 0, max: 40},  // green
            {strokeStyle: "#FF9903", min: 41, max: 60}, // orange
            {strokeStyle: "#CC0200", min: 61, max: 80} // red
        ];
        
        var colorValuesFuel = [
            {strokeStyle: "#CC0200", min: 0, max: 10},  // green
            {strokeStyle: "#FF9903", min: 11, max: 20}, // orange
            {strokeStyle: "#6AA84F", min: 21, max: 80} // red
        ];

        // options for gauge
        var optsSpeed = {
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
            staticZones: colorValuesSpeed
        };
        
        var optsRPM = {
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
            staticZones: colorValuesRPM
        };
        
        var optsTemp = {
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
            staticZones: colorValuesTemp
        };
        
        var optsOil = {
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
            staticZones: colorValuesOil
        };
        
        var optsAir = {
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
            staticZones: colorValuesAir
        };
        var optsFuel = {
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
            staticZones: colorValuesFuel
        };
        
        // setting gauges
        gaugeSpeed = new Gauge(targetSpeed).setOptions(optsSpeed);
        gaugeRPM = new Gauge(targetRPM).setOptions(optsRPM);
        gaugeTemp = new Gauge(targetTemp).setOptions(optsTemp);
        gaugeOil = new Gauge(targetOil).setOptions(optsOil);
        gaugeAir = new Gauge(targetAir).setOptions(optsAir);
        gaugeFuel = new Gauge(targetFuel).setOptions(optsFuel);

        // initializing gauge with values
        gaugeSpeed.set(0);
        gaugeRPM.set(0);
        gaugeTemp.set(0);
        gaugeOil.set(0);
        gaugeAir.set(0);
        gaugeFuel.set(0);
    }
    
});