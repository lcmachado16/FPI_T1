#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "operations.h"

#define ORIGINAL 0
#define NEW_IMAGE 1
#define GRAY_IMG 2
#define ORIGINAL_GRAY 3

using namespace cv;
using namespace std;

// /Users/lucasmachado/CIC/FPI/FPI/build

/*
 * ======= PRA COMPILAR ===========
 g++ -o main  main.cpp operations.cpp -std=c++11 $(pkg-config --cflags --libs opencv4)

*/

bool closeButtonClicked = false; // 

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


void reset_images(std::vector<cv::Mat>& vec_img){
    vec_img[NEW_IMAGE] = vec_img[ORIGINAL];
    vec_img[GRAY_IMG] = convert_rgb_to_gray(vec_img[ORIGINAL]);
}


bool gray_option_selected = false;
bool show_original = true;

void menu(cv::Mat& frame, std::vector<string>& strings , std::vector<cv::Mat>& vec_img, int& quant ){
    
    /* ============================================ *
     *   ATRIBUINDO VARIAVEIS PARA AS OPERACOES     *
     * ============================================ */ 
    cv::Mat3b img1 = vec_img[0];
    cv::Mat img2 = vec_img[1];
    cv::Mat1b gray_img = vec_img[2];

    string selected_img = strings[0];
    string selected_op = strings[1];
    string img_salva = strings[2];

    /* MENU */

    frame = cv::Scalar(180, 110, 110); //cor frame :: cor menu
    int x = 55;  int y = 15; int incY; //incremento utilizado para avancar o Y 
   
    /* TEXTOS Variaveis */
    // cvui::text(frame, 20 ,15, "Imagem selecionada: " , 0.5 , 0x120a8f);
    // cvui::text(frame, 200 ,15,  selected_img , 0.5 , 0x7a1b0c);

    cvui::text(frame, 20 ,50, "Efeito selecionado: " , 0.5 , 0x120a8f);
    cvui::text(frame, 200 ,50, selected_op , 0.5 , 0x7a1b0c);
    cvui::text(frame, x , 500, img_salva , 0.5, 0x120a8f);

    /* Botao Atualiza New Image */
    string msg_escolha_imagem = "";
    if (cvui::button(frame, 20, 75, "NewImage <= Original")) {
        gray_option_selected ?  vec_img[GRAY_IMG] = convert_rgb_to_gray(vec_img[ORIGINAL]) : img2 = img1;
    }
    /* =================== *
     *   DEMAIS BOTOES     *
     * =================== */ 
    y = 140;  incY = 40;
 
    cvui::text(frame, x-20,  y - 30, "Escolha a operacao ", 0.5 , 0x7a1b0c);

    /* HFLIP */
    if (cvui::button(frame, x, y, "Espelhar Horizontalmente")) {
        selected_op = "horizontal_flip";
        gray_option_selected ? vec_img[GRAY_IMG] = hflip(vec_img[GRAY_IMG]) : img2 = hflip(img2) ;
    }
    /* VFLIP */
    if (cvui::button(frame, x, y + incY, "Espelhar Verticalmente")) {
        selected_op = "vertical_flip";
        gray_option_selected ? vec_img[GRAY_IMG] = vflip(vec_img[GRAY_IMG]) : img2 = vflip(img2);
    }
    /*CONVERTER PRA CINZA*/
    if (cvui::button(frame, x,  y + 2*incY, "Converter para cinza")) {
        gray_option_selected = !gray_option_selected;
        gray_option_selected ? selected_op = "gray" : "";
    }
    
    /* BOTAO SALVAR IMAGEM */
    if (cvui::button(frame, x,  y + 3*incY, "Salvar Imagem")) {
        bool salvou = gray_option_selected ? save_image(vec_img[GRAY_IMG]) : save_image(vec_img[NEW_IMAGE]) ;
        img_salva = salvou ? strings[0] + strings[1] + strings[2]  : "" ; // reset de string
        mostrarMensagem("Salvo com sucesso!");
    }


     /* QUANTIZATION */
    if (cvui::button(frame, x,  y + 4*incY, "Quantizacao")) {
        selected_op = "Quantization" ;
        gray_option_selected = true;
        gray_img = convert_rgb_to_gray(vec_img[ORIGINAL]);
        quantization(gray_img, quant);
        vec_img[GRAY_IMG] = gray_img;
    }
    cvui::counter(frame, x + 120, y + 4.1*incY, &quant);
    cvui::trackbar(frame, x , y + 5*incY , 220, &quant,  0 , 255);


    /* BOTAO RESET */
    if (cvui::button(frame, 40,  550, "Reset")) {
        selected_op = "reset";
        vec_img[NEW_IMAGE] = vec_img[ORIGINAL];
        vec_img[GRAY_IMG] = convert_rgb_to_gray(vec_img[ORIGINAL]);
        gray_option_selected = false;
    }
    // Truncar o valor de Quant
    quant = std::max(0, std::min(255, quant));  

    /* REATRIBUINDO OS VALORES AO VETOR DE MATRIZES :: IMAGENS */
    vec_img[NEW_IMAGE] = img2;
    //vec_img[GRAY_IMG] = gray_img;
    strings[0] = selected_img; 
    strings[1] =selected_op;
    strings[2] = img_salva;
}



int main( int argc, char** argv )
{
    CommandLineParser parser( argc, argv, "{@input | Gramado_72k.jpg | input image}" );
    Mat image = imread( samples::findFile( parser.get<String>( "@input" ) ) );
    if( image.empty() )
    {
      cout << "Could not open or find the image!\n" << endl;
      cout << "Usage: " << argv[0] << " <Input image>" << endl;
      return -1;
    }

    cout  << "IARA " << parser.get<String>( "@input" ) << endl;



    /* DEFINICAO :: MATRIZES :: IMAGENS */
    cv::Mat new_image = Mat::zeros( image.size(), image.type() );
    cv::Mat1b new_image_gray = cv::Mat::zeros(image.size(), CV_8U);
    cv::Mat3b original_gray =  cv::Mat::zeros(image.size(), CV_8U);


    std::vector<cv::Mat> matriz_vetor = {image, new_image, new_image_gray, original_gray};
    
    /* =================== *
     *      MENU FRAME     *
     * =================== */ 
    cv::Mat frame = cv::Mat(600, 350, CV_8UC3);
    // Window inicialization
    cvui::init("Menu");



    /* QUANTIZACAO */
    int quant = 50;

    string selected_img = argc >= 2? parser.get<String>( "@input" )  : "Gramado.jpg";
    string selected_op = "";
    string save_sucess = "" ;
    std::vector<std::string> strings  = {selected_img, selected_op, save_sucess};

    matriz_vetor[ORIGINAL_GRAY] = convert_rgb_to_gray(matriz_vetor[ORIGINAL]);
    matriz_vetor[GRAY_IMG] = convert_rgb_to_gray(matriz_vetor[ORIGINAL]);
    matriz_vetor[NEW_IMAGE] = matriz_vetor[ORIGINAL];

    imshow("ORIGINAL", image);
    namedWindow("NEW_IMAGE");

  
    cv::moveWindow("ORIGINAL", 350, 100);
    cv::Rect tamanhoJanela = cv::getWindowImageRect("ORIGINAL");
    cv::moveWindow("NEW_IMAGE", 350 + tamanhoJanela.width, 100);

    while (true) {

        /* MENU */
        menu(frame,strings, matriz_vetor, quant);
        /* CINZA */
        cvui::text(frame, 20 ,30, "[GrayImage?]: " , 0.5 , 0x8B008b);
        cvui::checkbox(frame, 170, 30, "GrayImage?", &gray_option_selected, 0x8B008b);
        cvui::text(frame, 20 ,10, "[Color_Original?]: " , 0.5 , 0x8B008b);
        cvui::checkbox(frame, 170, 10, "ShowOriginal?", &show_original, 0x8B008b);

        imshow("ORIGINAL", show_original ? matriz_vetor[ORIGINAL] : matriz_vetor[ORIGINAL_GRAY]);
        imshow("NEW_IMAGE", gray_option_selected ? matriz_vetor[GRAY_IMG] : matriz_vetor[NEW_IMAGE]);

        /* REFRESH FRAME MENU */
        cvui::update();
        cv::imshow("Menu", frame);
       

        /* CHECK ESC KEY WAS PRESSED */ 
        if (cv::waitKey(20) == 27) {
            break;
        }
    }

    return 0;
}

