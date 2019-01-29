#Compilar projeto
gcc device_utils.c remote_database.c utils.h decode.c encode.c verify_live.c -lfprint -lwebsockets -l json-c -lcurl

>>Ambientação:

#must have installed:
sudo apt install libcurl4-openssl-dev
sudo apt install libjson-c-dev
sudo apt install libwebsockets-dev

Para instalar liblibfprint, em pasta /libfprint-0.7.0:
sudo ./configure; sudo make; sudo make install;

>>Erros:

##Quando não encontrar libusb-1.0:
Erro: 
	No package 'libusb-1.0' found

Solução: 
	sudo apt-get install libusb-1.0-0-dev

##Quando pkg-config for antigo:
Erro:
	Cannot find pkg-config error:
         
Solução:
	apt-get install -y pkg-config

##Quando não encontrar glib2.0:
Erro: 
	No package 'glib-2.0' found

Solução: 
	sudo apt install libgtk2.0-dev

##Quando não encontrar nss:
Erro: 
	No package 'nss' found

Solução: 
	sudo apt install libnss3-dev

##Quando não encontrar .o compilados:
Erro: ./a.out: error while loading shared libraries: libfprint.so.0: cannot open shared object file: No such file or directory
Solução:
	echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/usr_local_lib.conf
	sudo ldconfig
