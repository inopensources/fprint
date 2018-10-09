//JSON structure
// {
//    "type" : "DATA_RESPONSE",
//    "status" : "SUCCESS",
//    "method_name" : "get_user_list",
//    "message": "User list retrieved.",
//    "data" : "W3sidXN1YXJpb0lkIjo1Nywibm9tZSI6IkFORFJFIiwicGVyZm…lIjoiV0VOREVSU09OIiwicGVyZmlsIjoiUk9MRV9TVVAifV0="
// }
// $(document).ready(function () {
    let userId = 0;
    window.WebSocket = window.WebSocket || window.MozWebSocket;
    let websocket = new WebSocket('ws://localhost:8000',
        'dumb-increment-protocol');
    websocket.onopen = function () {
        // $('h1').css('color', 'green');
        console.log("Websocket opened.");
    };
    websocket.onerror = function () {
        $('#mainNav').css('color', 'green');
    };
    websocket.onmessage = function (message) {
        typeChecker(JSON.parse(message.data));
    };

    function id() {
        $(".user-card").click(function(e){
            userId = $(this).attr("user-id");
            rhCheckReset();
            on();
        });
    }

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
        switch (json.method_name) {
            case "cadastra_user":
                console.log("Cadastra user " + json.message);
                registerUser(json);
                break;
            case "enroll":
                console.log("Matriculando usuário " + json.message);
                startEnroll(json);
                break;
            case "do_point_final":
                console.log("Concluindo verificação " + json.message);
                dealWithReturnedUser(json);
                break;
            case "do_point":
                console.log("Verificando digital de usuário " + json.message);
                clockIn(json);
                break;
        }
    }

    function showErrorModal(json){
        $("#error-modal-description").html("<h5>"+json.message+"<h5>");
        $("#error-modal").modal('toggle');
    }

    function loadUserList(json) {
        var userList = JSON.parse(atob(json.data));
        $("#user-list").html("");

        for (i in userList) {
            var userRow = "<div class=\"col-sm-2 user-card\" user-id=\""+userList[i].usuarioId+"\" >\n" +
                "                    <div class=\"card\">\n" +
                "                        <img class=\"card-img-top\" src=\"img/user.jpg\" alt=\"Card image cap\">\n" +
                "                        <div class=\"card-body\" style=\"padding: 1px;\">\n" +
                "                            <h6>"+userList[i].nome+"</h6>\n" +
                "                        </div>\n" +
                "                    </div>\n" +
                "                </div>"
            $("#user-list").append(userRow);
            // if (i == 11){
            //     break;
            // }
        }
        id();
    }

    function registerUser(json){
        switch (json.status) {
            case "SUCCESS":
                updateEnrollStatus(json);
                break;
            case "ERROR":
                updateEnrollStatus(json);
                break;

        }
    }

    function clockIn(json){
        switch (json.status) {
            case "SUCCESS":
                updateDeviceStatus(json);
                break;
            case "ERROR":
                updateDeviceStatus(json);
                break;
        }
    }

// });