
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

    //O método abaixo exibe o overlay confirme user
    function onconfirm() {
        $("#confirm-register").slideDown(250);
    }

    //O método abaixo oculta o overlay confirme user
    function offconfirm() {
        $("#confirm-register").slideUp(250);
    }

    //O método abaixo inicia a verificação de digital do RH/Gerente
    function rhCheckStart(){
        $("#modal-rh-check-text").hide(250);
        $("#rh-check-device").show(250);
        $("#btn-verificar").hide(250);
        $("#img-fingerprint").show(250);
        verifyManagerFingerprint();
        //chamar verificação de digital de gerente/rh aqui
    }

    function rhCheckFinishSuccess(json){
        $("#img-fingerprint").hide(250);
        $("#modal-rh-check-text").html("<h2>"+json.message+"</h2>");
        $("#rh-check-device").hide();
        $("#modal-rh-check-text").show(250);
        setTimeout(function(){
            $("#img-fingerprint").show(250);
            setUserRegisterStage();
            registerFingerPrint(userId);
        }, 3000);
        resetDeviceStatus();
    }

    function rhCheckFinishError(json){
        $("#modal-rh-check-text").html("<h2>"+json.message+"</h2>");
        $("#rh-check-device").hide();
        $("#img-fingerprint").hide(250);
        $("#btn-verificar").show(250);
        $("#modal-rh-check-text").show(250);
        resetDeviceStatus();
    }

    //O método abaixo zera a verificação de digital
    function rhCheckReset(){
        $("#modal-rh-check-text").show(250);
        $("#modal-rh-register").html("<!-- Modal Header -->\n" +
            "                <div class=\"modal-header\">\n" +
            "                    <h2 class=\"modal-title rh-check-title\">Cadastro de Digital - Solicitação de Permissão</h2>\n" +
            "                    <button type=\"button\"  onclick=\"off()\" class=\"close\" data-dismiss=\"modal\">&times;</button>\n" +
            "                </div>\n" +
            "\n" +
            "                <!-- Modal body -->\n" +
            "                <div class=\"modal-body\">\n" +
            "                    <div id=\"modal-rh-check-text\">\n" +
            "                        <h3>\n" +
            "                             Você precisa ser um Gerente ou um representante do RH para cadastrar um usuário.\n" +
            "                        </h3>\n" +
            "                        <h3>\n" +
            "                             <b>Pressione o botão abaixo para iniciar a verificação</b>\n" +
            "                        </h3>\n" +
            "                    </div>\n" +
            "                    <div class=\"device-status\" id=\"rh-check-device\">\n" +
            "\n" +
            "                    </div>\n" +
            "                    <div class=\"user-register\" id=\"user-register\">\n" +
            "                    </div>\n" +
            "                    <div class=\"text-center\">\n" +
            "                        <a  id=\"btn-verificar\" class=\"btn btn-sq-xs\" onclick=\"resetScreens(); rhCheckStart()\">\n" +
            "                        <!--<a class=\"btn btn-sq-lg btn-primary\" onclick=\"registerFingerPrint(userId)\">-->\n" +
            "                            Verificar Permissão" +
            "                        </a>\n" +
            "                        <a  id=\"btn-cadastrar\" class=\"btn btn-sq-xs\" onclick=\"registerFingerPrint(userId);\" style=\"display: none; margin-top: 1.5rem;\">\n" +
            "                        <!--<a class=\"btn btn-sq-lg btn-primary\" onclick=\"registerFingerPrint(userId)\">-->\n" +
            "                            Cadastrar" +
            "                        </a>\n" +
            "                        <img id=\"img-fingerprint\" src=\"img/fingerprint.gif\" class=\"img-thumbnail\" style=\"height: 10rem\">"+
           /* "                        <i id=\"img-fingerprint\" class=\"fa fa-fingerprint\" style=\"font-size: 84px; padding: 20px;\"></i>\n" +*/
            "                    </div>\n" +
            "                </div>");

        $("#rh-check-device").hide(250);
        $("#img-fingerprint").hide(250);
    }

/*
    //O método abaixo solicita a confirmação do registro da digital
    function confirmRegister(){
        $("#modal-confirm-register").html("<!-- Modal Header -->\n" +
            "                <div class=\"modal-header\">\n" +
            "                    <h2 class=\"modal-title\">Confirmação de ponto de saída</h2>\n" +
            "                    <button type=\"button\"  onclick=\"offconfirm()\" class=\"close\" data-dismiss=\"modal\">&times;</button>\n" +
            "                </div>\n" +
            "\n" +
            "                <!-- Modal body -->\n" +
            "                <div class=\"modal-body\">\n" +
            "                    <div id=\"modal-rh-check-text\">\n" +
            "                        <h3>\n" +
            "                              Tem certeza que deseja bater o ponto de saída?\n" +
            "                        </h3>\n" +
            "                    </div>\n" +
            "                    <div class=\"text-center\">\n" +
            "                        <a class=\"btn btn-sq-xs\" onclick=\"verifyFingerprint()\">\n" +
            "                        <!--<a class=\"btn btn-sq-lg btn-primary\" onclick=\"registerFingerPrint(userId)\">-->\n" +
            "                            Sim" +
            "                        </a>\n" +
            "                        <a class=\"btn btn-sq-xs\" onclick=\"resetScreensAndGoHome();\" style=\"display: none; margin-top: 1.5rem;\">\n" +
            "                        <!--<a class=\"btn btn-
            "                        </a>\n" +
            "                </div>");
    }*/

    //O método abaixo atualiza as mensagens do dispositivo
    function updateDeviceStatus(json){
        $(".device-status").html(
            "<h2><b>"+json.message+"</b></h2>"
        )
    }

    //O método abaixo zera o status do dispositivo
    function resetDeviceStatus(){
        $(".device-status").html(
            "<h2></h2>"
        )
    }

    function resetRegisterComponents(){
        $("#img-fingerprint-registrar").hide(250);
        $("#btn-registrar-ponto").show(250);
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

    //O método abaixo atualiza o status do registro da digital
    function finishEnroll(json){
        $(".user-register").html(
            "<h2>"+json.message+"<h2>"
        );
    }

    //O método abaixo atualiza o status do registro da digital
    function startVerify(json){
        $(".user-register").html(
            "<h2>"+json.message+"<h2>"
        );
    }

    //O método abaixo atualiza o status do registro da digital
    function finishVerify(json){
        $(".user-register").html(
            "<h2>"+json.message+"<h2>"
        );
        setTimeout(function(){
            resetScreensAndGoHome();
        }, 5000);
    }


    //O método abaixo zera o status do registro da digital
    function resetEnrollStatus(json){
        $(".user-register").html(
            /*"<h2>Aguarde...<h2>"*/
        );
    }

    //O método abaixo limpa a lista de usuários
    function resetUserList() {
        $("#user-list").html(
            "<div class=\"container text-center\">" +
            "<img src=\"img/loading.gif\" class=\"img-fluid\" style=\"height: 10rem\">"+
            "<h2>Aguarde um pouco. Estamos carregando a lista de usuários...</h2>"+
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
        $("#btn-cadastrar").hide(250);
        $("#img-fingerprint").show(250);
        websocket.send("1 "+usuarioId);
    }

    //O método abaixo verifica a digital
    function verifyFingerprint() {
        /*confirmRegister()
        onconfirm();*/
        resetDeviceStatus();
        $("#img-fingerprint-registrar").show(250);
        $("#btn-registrar-ponto").hide(250);
        websocket.send("2");

    }

    //O método abaixo verifica a digital de um gerente/rh
    function verifyManagerFingerprint() {
        websocket.send("3");
    }

    function dealWithReturnedUser(json){
        var base = JSON.parse(atob(json.data));

        $(".user-entry").html(
            "<h1>Ponto de usuário</h1>" +
            "<img src=\"data:image/png;base64,"+base.usuario.foto+"\" class=\"img-thumbnail\" style=\"height: 10rem\">"+
            "<h2> Olá "+ base.usuario.nome+"</h2>"+
            "<h2>"+ utf8Decode(base.message)+"</h2>"+
            "</div>"
        );

        window.location.href = "#paginaUsuario";
    }

    function resetProfilePage(){
        $(".user-entry").html("");
    }

    function resetScreens(){
        resetProfilePage();
        // resetUserList();
        rhCheckReset();
        resetDeviceStatus();
    }

    function resetScreensAndGoHome(){
        off();
        resetProfilePage();
        rhCheckReset();
        resetDeviceStatus();
        window.location.href = "#boasVindas";
    }

    function setUserRegisterStage(){
        $("#modal-rh-check-text").hide(250);
        $(".rh-check-title").html("<h2 class='modal-title rg-check-title'>Cadastro de digital</h2>")
    }
