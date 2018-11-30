var myFullpage = new fullpage('#fullpage', {
    sectionsColor: ['#4A6FB1', '#a6a6a6', '#7BAABE', '#4A6FB1', '#C0C0C0'],
    anchors: ['boasVindas', 'controle', 'cadastraUsuario', 'batePonto', 'paginaUsuario'],
    menu: '#menu',

    //equivalent to jQuery `easeOutBack` extracted from http://matthewlein.com/ceaser/
    easingcss3: 'cubic-bezier(0.175, 0.885, 0.320, 1.275)',
    afterLoad: function (anchorLink, index) {
        switch (index.anchor) {
            case "boasVindas":
                resetScreens();
                break;
            case "batePonto":
                // Inicializando dispositivo para verificação
                //verifyFingerprint();
                break;
            case "cadastraUsuario":
                // Capturando lista de usuários
                resetUserList();
                resetDeviceStatus();
                returnUserList();
                id();
                break;
            case "paginaUsuario":
                setTimeout(function(){
                    window.location.href = "#boasVindas";
                }, 5000);
                break;
        }
    },
    onLeave: function (anchorLink, index) {
        switch (index.anchor) {
            case "batePonto":
                //Ao deixar a tela batePonto, o status é zerado
                resetDeviceStatus();
                resetRegisterComponents();
                break;
            case "cadastraUsuario":
                //Ao deixar a tela cadastraUsuario, a lista de usuários é zerada
                resetUserList();
                rhCheckReset();
                resetDeviceStatus();
                break;
        }
    }
});


// sleep(3000);
// window.location.href = "#boasVindas";
function sleep(milliseconds) {
    var start = new Date().getTime();
    for (var i = 0; i < 1e7; i++) {
        if ((new Date().getTime() - start) > milliseconds){
            break;
        }
    }
}