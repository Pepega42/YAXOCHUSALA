#define WIN32_LEAN_AND_MEAN // Этот макрос исключает редко используемые компоненты из заголовков Windows.h, ускоряя сборку

#include <Windows.h> // Заголовочный файл для работы с API Windows
#include <iostream> // Заголовочный файл для ввода/вывода через консоль
#include <WinSock2.h> // Заголовочный файл для работы с сокетами в Windows
#include <WS2tcpip.h> // Заголовочный файл для работы с TCP/IP протоколами

using namespace std;
int main()
{
    WSADATA wsaData;  // Структура, содержащая информацию о реализации сокетов Windows
    ADDRINFO hints; // структура, которая хранит параметры для функции getaddrinfo
    ADDRINFO* addrResult; // Указатель на результаты функции getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для соединения с сервером
    const char* sendBuffer = "Hello from Client"; // Буфер с данными для отправки
    const char* sendBuffer2 = "Hello from Client 2"; // Буфер с данными для отправки
    char recvBuffer[512]; // Буфер для приема данных


    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация использования библиотеки Winsock
    if (result != 0) {
        cout << "WSAStartup failed result" << endl;
        WSACleanup();
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуление структуры hints
    hints.ai_family = AF_INET; // IPv4 адресация
    hints.ai_socktype = SOCK_STREAM; // Тип сокета - потоковый
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    result = getaddrinfo("localhost", "666", &hints, &addrResult); // Выполнение функции getaddrinfo для получения информации об адресе сервера по имени "localhost" и порту "666"
    if (result != 0) {
        cout << "getaddrinfo failed error" << endl; // Вывод сообщения об ошибке, если getaddrinfo завершилась неудачно, с указанием кода ошибки
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Возвращение кода ошибки 1, чтобы указать неудачное выполнение программы
    }


    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);// Создание сокета с помощью функции socket, используя параметры из структуры addrResult
    if (ConnectSocket == INVALID_SOCKET) { // Проверка, был ли создан сокет успешно
        cout << "Socket creation with" << endl; // Вывод сообщения об ошибке в случае неудачного создания сокета
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Возвращение кода ошибки 1, чтобы указать неудачное выполнение программы
    }
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Попытка установить соединение с сервером, используя созданный сокет и адрес сервера из структуры addrResult
    if (result == SOCKET_ERROR) { // Проверка результата попытки установить соединение
        cout << "Unable connect to server" << endl; // Вывод сообщения об ошибке в случае неудачного соединения
        ConnectSocket = INVALID_SOCKET; // Присвоение сокету значения INVALID_SOCKET для указания на неудачное соединение
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Возвращение кода ошибки 1, чтобы указать неудачное выполнение программы
    }
    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // Попытка отправить данные на сервер через сокет ConnectSocket. sendBuffer1 содержит данные для отправки, strlen(sendBuffer1) - размер данных в байтах.
    if (result == SOCKET_ERROR) {
        cout << "Send failed error " << endl; // Вывод сообщения об ошибке и кода ошибки в случае неудачной отправки данных
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Возвращение кода ошибки 1, чтобы указать неудачное выполнение программы
    }
    cout << "Sent: " << result << "bytes" << endl; // Вывод количества успешно отправленных байт данных

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0); // Попытка отправить второй набор данных на сервер через сокет ConnectSocket. sendBuffer2 содержит данные для отправки, strlen(sendBuffer2) - размер данных в байтах.
    if (result == SOCKET_ERROR) { // Проверка результата отправки второго набора данных
        cout << "Send failed error " << endl; // Вывод сообщения об ошибке и кода ошибки в случае неудачной отправки второго набора данных
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Возвращение кода ошибки 1, чтобы указать неудачное выполнение программы
    }
    cout << "Sent: " << result << "bytes" << endl; // Вывод количества успешно отправленных байт данных второго набора


    result = shutdown(ConnectSocket, SD_SEND); // Отправка сигнала серверу о закрытии отправки данных с помощью функции shutdown. ConnectSocket - сокет для отправки сигнала, SD_SEND - указывает, что отправка данных должна быть закрыта.
    if (result == SOCKET_ERROR) { // Проверка результата выполнения функции shutdown
        cout << "shutdown error " << endl; // Вывод сообщения об ошибке и кода ошибки в случае неудачного выполнения shutdown
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Возвращение кода ошибки 1, чтобы указать неудачное выполнение программы
    }
    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера приема для очистки предыдущих данных
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Прием данных от сервера. ConnectSocket - сокет для приема данных, recvBuffer - буфер для сохранения принятых данных, 512 - максимальное количество байт для приема, 0 - флаги (в данном случае без дополнительных флагов)
        if (result > 0) { // Если данные успешно приняты
            cout << "Received " << result << "bytes" << endl; // Вывод количества принятых байт
            cout << "Received data " << recvBuffer << endl; // Вывод принятых данных
        }
        else if (result == 0) // Если соединение закрыто сервером
        {
            cout << "Connection closed" << endl; // Вывод сообщения о закрытии соединения
        }
        else // Если произошла ошибка при приеме данных
        {
            cout << "Recieved failed  " << endl; // Вывод сообщения об ошибке и кода ошибки
        }
    } while (result > 0); // Повторять цикл, пока данные успешно принимаются
    freeaddrinfo(addrResult); // Очистка структуры ADDRINFO
    WSACleanup(); // Выгрузка библиотеки Winsock
    return 0;
}