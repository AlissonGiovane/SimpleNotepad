#include <windows.h>
#include <commdlg.h> // Para o diálogo de abertura e salvamento de arquivos
#include <stdio.h>

#define ID_EDIT 1
#define ID_FILE_NEW 1001
#define ID_FILE_OPEN 1002
#define ID_FILE_SAVE 1003
#define ID_FILE_EXIT 1004

// Função para abrir um arquivo e carregar seu conteúdo na área de edição
void OpenFile(HWND hEdit) {
    OPENFILENAME ofn;       // Estrutura para o diálogo de abertura de arquivos
    WCHAR szFile[260];     // Buffer para o nome do arquivo

    // Inicializa a estrutura OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn)); // Limpa a estrutura
    ofn.lStructSize = sizeof(ofn); // Define o tamanho da estrutura
    ofn.hwndOwner = hEdit; // Define a janela que possui o diálogo
    ofn.lpstrFile = szFile; // Ponteiro para o buffer do nome do arquivo
    ofn.lpstrFile[0] = '\0'; // Inicializa o buffer
    ofn.nMaxFile = sizeof(szFile); // Tamanho máximo do buffer
    ofn.lpstrFilter = L"Text Documents (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; // Filtros para os tipos de arquivos
    ofn.nFilterIndex = 1; // Filtro padrão
    ofn.lpstrFileTitle = NULL; // Título do arquivo (não usado aqui)
    ofn.nMaxFileTitle = 0; // Tamanho máximo do título (não usado aqui)
    ofn.lpstrInitialDir = NULL; // Diretório inicial
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // Flags para garantir que o caminho e o arquivo existem

    // Abre o diálogo de abertura de arquivos
    if (GetOpenFileName(&ofn)) {
        // Lê o arquivo e coloca o conteúdo na área de edição
        HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = GetFileSize(hFile, NULL); // Obtém o tamanho do arquivo
            char* buffer = (char*)GlobalAlloc(GPTR, fileSize + 1); // Aloca memória para o buffer
            DWORD bytesRead;

            ReadFile(hFile, buffer, fileSize, &bytesRead, NULL); // Lê o conteúdo do arquivo
            buffer[fileSize] = '\0'; // Termina a string

            SetWindowTextA(hEdit, buffer); // Coloca o texto na área de edição
            GlobalFree(buffer); // Libera a memória alocada
            CloseHandle(hFile); // Fecha o handle do arquivo
        }
    }
}

// Função para salvar o conteúdo da área de edição em um arquivo
void SaveFile(HWND hEdit) {
    OPENFILENAME ofn;       // Estrutura para o diálogo de salvamento de arquivos
    WCHAR szFile[260];     // Buffer para o nome do arquivo

    // Inicializa a estrutura OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn)); // Limpa a estrutura
    ofn.lStructSize = sizeof(ofn); // Define o tamanho da estrutura
    ofn.hwndOwner = hEdit; // Define a janela que possui o diálogo
    ofn.lpstrFile = szFile; // Ponteiro para o buffer do nome do arquivo
    ofn.lpstrFile[0] = '\0'; // Inicializa o buffer
    ofn.nMaxFile = sizeof(szFile); // Tamanho máximo do buffer
    ofn.lpstrFilter = L"Text Documents (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; // Filtros para os tipos de arquivos
    ofn.nFilterIndex = 1; // Filtro padrão
    ofn.lpstrFileTitle = NULL; // Título do arquivo (não usado aqui)
    ofn.nMaxFileTitle = 0; // Tamanho máximo do título (não usado aqui)
    ofn.lpstrInitialDir = NULL; // Diretório inicial
    ofn.Flags = OFN_OVERWRITEPROMPT; // Flag para pedir confirmação se o arquivo existe

    // Abre o diálogo de salvamento de arquivos
    if (GetSaveFileName(&ofn)) {
        // Salva o conteúdo da área de edição no arquivo
        HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            int length = GetWindowTextLengthA(hEdit); // Obtém o comprimento do texto na área de edição
            char* buffer = (char*)GlobalAlloc(GPTR, length + 1); // Aloca memória para o buffer
            GetWindowTextA(hEdit, buffer, length + 1); // Obtém o texto da área de edição

            DWORD bytesWritten;
            WriteFile(hFile, buffer, length, &bytesWritten, NULL); // Escreve o texto no arquivo

            GlobalFree(buffer); // Libera a memória alocada
            CloseHandle(hFile); // Fecha o handle do arquivo
        }
    }
}

// Função de procedimento da janela
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY: // Mensagem de destruição da janela
        PostQuitMessage(0); // Encerra o loop de mensagem
        return 0;
    case WM_CREATE: {
        // Criando um controle de edição
        HWND hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, // Estilo da janela
            L"EDIT", // Tipo de controle
            NULL, // Texto inicial
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE, // Estilos do controle
            0, 0, 800, 600, // Posição e tamanho
            hwnd, // Janela pai
            (HMENU)ID_EDIT, // Identificador do controle
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), // Instância do aplicativo
            NULL // Parâmetro adicional
        );
        // Ajusta a fonte do controle de edição
        SendMessage(hEdit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    }
                  return 0;
    case WM_SIZE: { // Mensagem de redimensionamento da janela
        HWND hEdit = GetDlgItem(hwnd, ID_EDIT); // Obtém o controle de edição
        if (hEdit) {
            RECT rcClient; // Estrutura para armazenar o tamanho da janela
            GetClientRect(hwnd, &rcClient); // Obtém as dimensões da janela
            SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER); // Ajusta o controle de edição
        }
    }
                return 0;
    case WM_COMMAND: { // Mensagem de comando (ações do menu)
        switch (LOWORD(wParam)) {
        case ID_FILE_NEW: // Novo arquivo
            // Limpa a área de edição
            SetWindowText(GetDlgItem(hwnd, ID_EDIT), L"");
            break;
        case ID_FILE_OPEN: // Abrir arquivo
            OpenFile(GetDlgItem(hwnd, ID_EDIT)); // Chama a função de abrir
            break;
        case ID_FILE_SAVE: // Salvar arquivo
            SaveFile(GetDlgItem(hwnd, ID_EDIT)); // Chama a função de salvar
            break;
        case ID_FILE_EXIT: // Sair
            PostMessage(hwnd, WM_DESTROY, 0, 0); // Encerra o aplicativo
            break;
        }
    }
                   return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); // Processa mensagens padrão
}

// Função principal
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd) {
    WNDCLASS wc = { 0 }; // Inicializa a classe da janela
    wc.lpfnWndProc = WindowProc; // Define a função de procedimento da janela
    wc.hInstance = hInstance; // Define a instância do aplicativo
    wc.lpszClassName = L"SimpleNotepadClass"; // Nome da classe da janela
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Cursor padrão
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Cor de fundo da janela

    RegisterClass(&wc); // Registra a classe da janela

    // Criar a janela principal
    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName, // Nome da classe
        L"Notepad Simples", // Título da janela
        WS_OVERLAPPEDWINDOW, // Estilo da janela
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // Posição e tamanho
        NULL, NULL, hInstance, NULL // Janela pai, menu, instância, parâmetro adicional
    );

    // Criando o menu
    HMENU hMenu = CreateMenu(); // Cria um novo menu
    HMENU hFileMenu = CreateMenu(); // Cria um submenu "File"

    // Adiciona opções ao menu "File"
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_NEW, L"Novo\tCtrl+N"); // Novo
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, L"Abrir\tCtrl+O"); // Abrir
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, L"Salvar\tCtrl+S"); // Salvar
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL); // Separador
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Sair\tAlt+F4"); // Sair

    // Adiciona o submenu "File" ao menu principal
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Arquivo");
    SetMenu(hwnd, hMenu); // Define o menu da janela

    ShowWindow(hwnd, nShowCmd); // Exibe a janela
    UpdateWindow(hwnd); // Atualiza a janela

    // Loop de mensagens da aplicação
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Traduz as mensagens
        DispatchMessage(&msg); // Processa as mensagens
    }

    return 0; // Retorna 0 para encerrar o aplicativo
}
