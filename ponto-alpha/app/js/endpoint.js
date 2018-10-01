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
        console.log(message.data.substr(0, 3));

        switch (message.data.substr(0, 3)) {
            case "LUS":
                console.log("User list loaded");
                loadUserList(message.data.substr(4, message.data.lenght));
                break;
        }
    };

    function returnUserList() {
        websocket.send("0");
    }

    $('button').click(function (e) {
        e.preventDefault();
        websocket.send($('input').val());
        $('input').val('');
    });

    function returnUserList() {
        websocket.send("0");
    }

    function registerFingerPrint(usuarioId) {
        websocket.send("1 "+usuarioId);
    }

    function verifyFingerprint(usuarioId) {
        websocket.send("2");
    }

    function loadUserList(json) {
        var userList = JSON.parse(json);
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


});