#include <opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include "../cvui.h"

bool closeButtonClicked = false;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        // Verificar se o clique está dentro da área do botão de fechar
        if (x > 100 && x < 200 && y > 50 && y < 100) {
            closeButtonClicked = true;
        }
    }
}

void mostrarMensagem(const std::string& mensagem) {
    // Criar uma janela para exibir a mensagem
    cv::Mat messageWindow = cv::Mat::zeros(150, 300, CV_8UC3);
    messageWindow= cv::Scalar(100,150,150);
    //cv::putText(messageWindow, mensagem, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
    cvui::text(messageWindow, 30 ,15, mensagem , 0.7 , 0x120a8f);
    // Adicionar um botão de fechar usando a cvui
    if (cvui::button(messageWindow, 100, 50, 100, 50, "Fechar")) {
        closeButtonClicked = true;
    }

    // Exibir a mensagem em uma nova janela
    cvui::imshow("Mensagem", messageWindow);

    // Configurar o callback do mouse para detectar cliques no botão de fechar
    cv::setMouseCallback("Mensagem", onMouse, nullptr);

    // Loop principal para esperar até que o botão de fechar seja clicado
    while (!closeButtonClicked) {
        cvui::update();  // Atualizar a interface cvui
        int key = cv::waitKey(10);
        if (key == 27) {  // Espera até que a tecla Esc seja pressionada para sair
            break;
        }
    }
    // Resetar a variável de controle
    closeButtonClicked = false;
    // Destruir a janela
    cv::destroyWindow("Mensagem");
}

int main() {
    // Inicializar a biblioteca cvui
    cv::Mat frame = cv::Mat(600, 800, CV_8UC3);
    cvui::init("Mensagem");

    // Chamar a função que mostra a mensagem
    mostrarMensagem("Salvo com sucesso!");

    // Chamar a função novamente com uma mensagem diferente
    mostrarMensagem("Outra mensagem importante!");

    return 0;
}
