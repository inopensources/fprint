//JSON structure
// {
//    "type" : "DATA_RESPONSE",
//    "status" : "SUCCESS",
//    "method_name" : "get_user_list",
//    "message": "User list retrieved.",
//    "data" : "W3sidXN1YXJpb0lkIjo1Nywibm9tZSI6IkFORFJFIiwicGVyZm…lIjoiV0VOREVSU09OIiwicGVyZmlsIjoiUk9MRV9TVVAifV0="
// }
$(document).ready(function () {
    window.WebSocket = window.WebSocket || window.MozWebSocket;
    let websocket = new WebSocket('ws://localhost:8000',
        'dumb-increment-protocol');
    websocket.onopen = function () {
        $('h1').css('color', 'green');
    };
    websocket.onerror = function () {
        $('#mainNav').css('color', 'green');
    };
    websocket.onmessage = function (message) {
        console.log("Message received,");
        typeChecker(JSON.parse(message.data));
    };


    $('button').click(function (e) {
        e.preventDefault();
        websocket.send($('input').val());
        $('input').val('');
    });


    $('#load-user-list').click(function (e) {
        returnUserList();
    });

    $('#read-digital').click(function (e) {
        verifyFingerprint();
    });

    function id() {
        $('.user-list').click(function (e) {
            registerFingerPrint($(this).attr("id"));
        });
    }

    //For test purpose only
    $('button').click(function (e) {
        e.preventDefault();
        websocket.send($('input').val());
        $('input').val('');
    });

    //The method below defines the execution flow that's going to be used according to the
    //message received from the C backend
    function typeChecker(json){
        console.log(json);
        switch (json.type){
            case "DATA_RESPONSE":
                console.log("Data received " + json.message);
                dataResponse(json);
                break;
            case "SCREEN_UPDATE":
                console.log("Screen update command received " + json.message);
                screenUpdate(json);
                break;
            case "CONSOLE_LOG":
                console.log("Console log command received " + json.message);
                break;
            default:
                console.log("Don't know what do |: \n" + JSON.stringify(json));
        }
    }

    function dataResponse(json) {
        switch (json.method_name) {
            case "get_user_list":
                console.log("User list received" + json.message);
                loadUserList(json);
                break;
        }
    }

    function screenUpdate(json){
        alert(json.message);
    }

    function loadUserList(json) {
        var userList = JSON.parse(atob(json.data));
        $("#user-list-table").html("");

        for (i in userList) {
            var userRow = '<tr class="user-list" id="' + userList[i].usuarioId + '">' +
                '<td>' + userList[i].usuarioId + '</td>' +
                '<td>' + userList[i].nome + '</td>' +
                '</tr>'
            $("#user-list-table").append(userRow);
        }
        id();
    }

    function returnUserList() {
        websocket.send("0");
    }

    function registerFingerPrint(usuarioId) {
        websocket.send("1 "+usuarioId);
    }

    function verifyFingerprint(usuarioId) {
        websocket.send("2");
    }


});