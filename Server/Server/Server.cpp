﻿#define WIN32_LEAN_AND_MEAN // макрос, который указывает на то, что мы хотим использовать только необходимые функции из Windows API

#include <iostream> // Заголовочный файл для работы с API Windows
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h> // заголовочный файл, содержащий дополнительные функции для работы с TCP/IP
using namespace std;

int main()
{
    WSADATA wsaData; // структура, которая хранит информацию о версии Windows Sockets
    SOCKET СonnectSocket = INVALID_SOCKET; // сокет для подключения клиента
    SOCKET ListenSocket = INVALID_SOCKET; // слушающий сокет, ожидающий входящие соединения
    ADDRINFO hints; // структура, которая хранит параметры для функции getaddrinfo
    ADDRINFO* addrResult; // указатель на структуру, которая будет содержать результат функции getaddrinfo
    const char* sendBuffer = "Hello from Server"; // буфер для отправки данных клиенту
    char recvBuffer[512]; // буфер для принятия данных от клиента

    // старт использования библиотеки сокетов процессом определния версии и структуры
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // Если произошла ошибка подгрузки библиотеки, выводим сообщение и завершаем выполнение программы
    if (result != 0) {
        cout << "WSAStartup failed result" << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));//нам необходимо изначально занулить память, 1-ый паметр, что зануляем,2-ой сколько
    hints.ai_family = AF_INET;//4-БАЙТНЫЙ Ethernet
    hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP;// Используем протокол TCP
    hints.ai_flags = AI_PASSIVE;// Пассивная сторона, потому что просто ждет соединения

    // функциия хранит в себе адрес, порт,семейство структур, адрес сокета
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    // Если инициализация структуры адреса завершилась с ошибкой,
    // выведем сообщением об этом и завершим выполнение программы 
    if (result != 0) {
        cout << "getaddrinfo failed with error" << endl;
        WSACleanup();//очситка WSAStartup
        return 1;
    }


    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    // Если создание сокета завершилось с ошибкой, выводим сообщение,
      // освобождаем память, выделенную под структуру addr,
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket error" << endl;
        WSACleanup();//очситка WSAStartup
        freeaddrinfo(addrResult);
        return 1;
    }
    // Привязываем сокет к IP-адресу (соединились с сервером)
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    // Если привязать адрес к сокету не удалось, то выводим сообщение
      // об ошибке, освобождаем память, выделенную под структуру addr.
      // и закрываем открытый сокет.
      // Выгружаем DLL-библиотеку из памяти и закрываем программу.
    if (result == SOCKET_ERROR) {
        cout << "Binding connect failed <3" << endl;
        closesocket(СonnectSocket);
        ListenSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();//очситка WSAStartup
        return 1;
    }
    // Переводим сокет в режим прослушивания для входящих соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Listening failed" << endl;
        closesocket(СonnectSocket);
        ListenSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принимаем входящее соединение
    СonnectSocket = accept(ListenSocket, NULL, NULL);
    if (СonnectSocket == INVALID_SOCKET) {
        cout << "Accepting SOCKET failed" << endl;
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрываем слушающий сокет, так как он больше не нужен
    closesocket(ListenSocket);

    do {
        // Обнуляем буфер для принятия данных
        ZeroMemory(recvBuffer, 512);
        // Принимаем данные от клиента
        result = recv(СonnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << "bytes" << endl;
            cout << "Received data " << recvBuffer << endl;

            // Отправляем данные клиенту
            result = send(СonnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "failed to send" << endl;
                WSACleanup();//очситка WSAStartup
                return 1;
            }
        }
        else if (result == 0)
            cout << "Connection closing" << endl;

        else {
            cout << "recv failed with error" << endl;
            closesocket(СonnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }

    } while (result > 0);

    // Завершаем соединение сокета
    result = shutdown(СonnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрываем сокет
    closesocket(СonnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;




}