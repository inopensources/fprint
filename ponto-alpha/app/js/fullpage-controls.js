var myFullpage = new fullpage('#fullpage', {
    sectionsColor: ['#4A6FB1', '#a6a6a6', '#7BAABE', '#4A6FB1', '#4A6FB1'],
    anchors: ['boasVindas', 'controle', 'cadastraUsuario', 'batePonto', 'lastPage'],
    menu: '#menu',

    //equivalent to jQuery `easeOutBack` extracted from http://matthewlein.com/ceaser/
    easingcss3: 'cubic-bezier(0.175, 0.885, 0.320, 1.275)',
    afterLoad: function (anchorLink, index) {
        switch (index.anchor) {
            case "batePonto":
                // Inicializando dispositivo
                verifyFingerprint();
                break;
            case "cadastraUsuario":
                // Capturando lista de usuários
                resetUserList();
                resetDeviceStatus();
                returnUserList();
                id();
                break;
        }
    },
    onLeave: function (anchorLink, index) {
        switch (index.anchor) {
            case "batePonto":
                //Ao deixar a tela batePonto, o status é zerado
                resetDeviceStatus();
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