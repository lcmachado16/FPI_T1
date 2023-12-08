#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "operations.h"

#define ORIGNAL 0
#define NEW_IMAGE 1
#define GRAY_IMG 2

using namespace cv;
using namespace std;

// /Users/lucasmachado/CIC/FPI/FPI/build

/*
 * ======= PRA COMPILAR ===========
 g++ -o main  main.cpp operations.cpp -std=c++11 $(pkg-config --cflags --libs opencv4)

*/


void reset_images(std::vector<cv::Mat>& vec_img){
    vec_img[NEW_IMAGE] = vec_img[ORIGNAL];
    vec_img[GRAY_IMG] = convert_rgb_to_gray(vec_img[ORIGNAL]);
}


bool gray_option_selected = false;

bool save_image(cv::Mat& img){

     if (img.empty()) {
        std::cerr << "Erro ao carregar a imagem" << std::endl;
        return false;
    }
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(97); //qualidade 97, padrao eh 95
    try {
        imwrite("dst_images/result.jpg", img, compression_params);
        printf("IMAGEM SALVA COM SUCESSO");
    } catch (runtime_error& ex) {
        fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
        return false;
    }
    return true;
}





void menu(cv::Mat& frame, string& selected_img, string& selected_op, std::vector<cv::Mat>& vec_img, int& quant ){
    
    /* ============================================ *
     *   ATRIBUINDO VARIAVEIS PARA AS OPERACOES     *
     * ============================================ */ 
    cv::Mat3b img1 = vec_img[0];
    cv::Mat img2 = vec_img[1];
    cv::Mat1b gray_img = vec_img[2];

    /* MENU */

    frame = cv::Scalar(180, 110, 110); //cor frame :: cor menu
    int x = 55;  int y = 15; int incY; //incremento utilizado para avancar o Y 
   
    /* TEXTOS Variaveis */
    cvui::text(frame, 20 ,15, "Imagem selecionada: " , 0.5 , 0x120a8f);
    cvui::text(frame, 200 ,15,  selected_img , 0.5 , 0x7a1b0c);

    cvui::text(frame, 20 ,30, "Efeito selecionado: " , 0.5 , 0x120a8f);
    cvui::text(frame, 200 ,30, selected_op , 0.5 , 0x7a1b0c);

    /* Botao Atualiza New Image */
    string msg_escolha_imagem = "";
    if (cvui::button(frame, 20, 75, "NewImage <= Original")) {
        !gray_option_selected ? img2 = img1 : gray_img = convert_rgb_to_gray(vec_img[NEW_IMAGE]);
    }
    /* =================== *
     *   DEMAIS BOTOES     *
     * =================== */ 
    // Escolha a foto a ser carregada
    y = 135;  incY = 40;
    // cvui::text(frame, x-20,y - incY/2, "Escolha a foto a ser carregada ", 0.5 , 0x7a1b0c);
    // if (cvui::button(frame, x, y, "Gramado_72k.jpg")) {
    //     // selected_img = gramado;
    // }
    // if (cvui::button(frame, x, y + incY, "Space_187k.jpg")) {
    //     // selected_img = space;
    // }
    // if (cvui::button(frame, x, y + 2*incY, "Underwater_53k.jpg")) {
    //     // selected_img = underwater;
    // }
    
    /* ESCOLHA A OPERACAO */
    y = y + 160;
    cvui::text(frame, x-20,  y - 30, "Escolha a operacao ", 0.5 , 0x7a1b0c);

    /* HFLIP */
    if (cvui::button(frame, x, y, "Espelhar Horizontalmente")) {
        selected_op = "horizontal_flip";
        gray_option_selected ? img2 = vec_img[GRAY_IMG] = hflip(vec_img[GRAY_IMG]) : img2 = hflip(img2) ;
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
        bool salvou = gray_option_selected ? save_image( vec_img[GRAY_IMG] )  : save_image(vec_img[NEW_IMAGE]) ;
    }


     /* QUANTIZATION */
    if (cvui::button(frame, x,  y + 4*incY, "Quantizacao")) {
        selected_op = "Quantization" ;
        gray_option_selected = true;
        quantization(gray_img, quant);
    }
    cvui::counter(frame, x + 120, y + 4.1*incY, &quant);
    cvui::trackbar(frame, x , y + 5*incY , 220, &quant,  0 , 255);


    /* BOTAO RESET */
    if (cvui::button(frame, 40,  550, "Reset")) {
        selected_op = "reset";
        vec_img[NEW_IMAGE] = vec_img[ORIGNAL];
        vec_img[GRAY_IMG] = convert_rgb_to_gray(vec_img[ORIGNAL]);
        gray_option_selected = false;
    }


    /* REATRIBUINDO OS VALORES AO VETOR DE MATRIZES :: IMAGENS */
    vec_img[NEW_IMAGE] = img2;
    //vec_img[GRAY_IMG] = gray_img;
    quant = std::max(0, std::min(255, quant)); // Truncar o valor de Quant 
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
    cv::Mat3b img1 = image;
    cv::Mat img2 = Mat::zeros( image.size(), image.type() );
    cv::Mat1b cinza = cv::Mat::zeros(image.size(), CV_8U);


    std::vector<cv::Mat> matriz_vetor = {img1, img2, cinza};
    
    /* =================== *
     *      MENU FRAME     *
     * =================== */ 
    cv::Mat frame = cv::Mat(600, 350, CV_8UC3);
    // Window inicialization
    cvui::init("Menu");



    /* QUANTIZACAO */
    int quant = 50;

    string selected_img = argc >= 2 ? argv[1] : "Gramado.jpg";
    string selected_op = "";

    matriz_vetor[GRAY_IMG] = convert_rgb_to_gray(matriz_vetor[ORIGNAL]);
    matriz_vetor[NEW_IMAGE] = matriz_vetor[ORIGNAL];

    while (true) {

        /* MENU */
        menu(frame,selected_img,selected_op, matriz_vetor, quant);
        /* CINZA */
        cvui::text(frame, 20 ,50, "CHECKBOX CINZA: " , 0.5 , 0x8B008b);
        cvui::checkbox(frame, 170, 50, "CINZA?", &gray_option_selected, 0x8B008b);

        // /* IMAGEM 1 */
        //load_image(path_string, selected_img, matriz_vetor[IMG1]); // Recarrega imagem :: util ao selecionar outra opcao
        // imshow("Original", matriz_vetor[IMG2]);

        imshow("ORIGINAL", image);
        imshow("NEW IMAGE", !gray_option_selected ? matriz_vetor[NEW_IMAGE] : matriz_vetor[GRAY_IMG]);
        // imshow("NEW IMAGE",matriz_vetor[GRAY_IMG]);

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


/* TO DOS
    * Configurar selecao de arquivo pra MacOS e fora de MacOS :: Load images 
    * Quantizar imagem em tons de cinza
    * A funcao load_image foi concebida com base nos diretorios locais do MacOS => é preciso alterar isto
    * 
    * Boas Praticas :: 
    *  passar variaveis globais por referencia 
    *  criar um arquivo separado para funcoes auxiliares 
    * Passar variaveis como cor para globais (nao sei se é possivel pq nao sao strings) :: 0x120a8f 
*/