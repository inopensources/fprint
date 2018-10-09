
    //O método abaixo exibe o overlay
    function on() {
        $("#overlay").slideDown(250);
        // document.getElementById("overlay").style.display = "block";
    }

    //O método abaixo oculta o overlay
    function off() {
        $("#overlay").slideUp(250);
        // document.getElementById("overlay").style.display = "none";
    }

    //O método abaixo inicia a verificação de digital do RH/Gerente
    function rhCheckStart(){
        $("#modal-rh-check-text").hide(250);
        $("#rh-check-device").show(250);
        verifyManagerFingerprint();
        //chamar verificação de digital de gerente/rh aqui
    }

    //O método abaixo zera a verificação de digital
    function rhCheckReset(){
        $("#modal-rh-check-text").show(250);
        $("#rh-check-device").hide(250);
    }

    //O método abaixo atualiza as mensagens do dispositivo
    function updateDeviceStatus(json){
        $(".device-status").html(
            "<h2><b>"+json.message+"</b></h2>"
        )
    }

    //O método abaixo zera o status do dispositivo
    function resetDeviceStatus(){
        $(".device-status").html(
            "<h2><b>Aguarde...</b></h2>"
        )
    }

    //O método abaixo atualiza o status do registro da digital
    function startEnroll(json){
        $(".user-register").html(
            "<h2>"+json.message+"<h2>"
        );
    }

    //O método abaixo atualiza o status do registro da digital
    function updateEnrollStatus(json){
        $(".user-register").html(
            "<h2>"+json.message+"<h2>"
        );
    }

    //O método abaixo zera o status do registro da digital
    function resetEnrollStatus(json){
        $(".user-register").html(
            "<h2>Aguarde...<h2>"
        );
    }

    //O método abaixo limpa a lista de usuários
    function resetUserList() {
        $("#user-list").html(
            "<div class=\"container text-center\">" +
            "<img src=\"img/loading.gif\" class=\"img-fluid\" style=\"height: 10rem\">"+
            "<h2>Carregando lista de usuários...</h2>"+
            "</div>"
        );
    }

    //O método abaixo requisita uma lista de usuários
    function returnUserList() {
        resetUserList();
        websocket.send("0");
    }

    //O método abaixo inicia o registro de digital
    function registerFingerPrint(usuarioId) {
        websocket.send("1 "+usuarioId);
    }

    //O método abaixo verifica a digital
    function verifyFingerprint() {
        websocket.send("2");
    }

    //O método abaixo verifica a digital de um gerente/rh
    function verifyManagerFingerprint() {
        websocket.send("3");
    }

    function dealWithReturnedUser(json){
        // var base = atob(json.data);
        var base = JSON.parse(atob(json));
        alert(JSON.stringify(base));
        $(".device-status").html(
            "<h2><b>"+base.name+"</b></h2>"
        )
    }