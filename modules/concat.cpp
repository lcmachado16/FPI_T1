#include <opencv2/opencv.hpp>

int main() {
    // Carregar duas imagens
    cv::Mat image1 = cv::imread("/Users/lucasmachado/CIC/FPI/trabalho_1/Gramado_72k.jpg");
    cv::Mat image2 = cv::imread("/Users/lucasmachado/CIC/FPI/trabalho_1/Gramado_72k.jpg");

    if (image1.empty() || image2.empty()) {
        std::cerr << "Erro ao carregar as imagens!" << std::endl;
        return -1;
    }

    // Verificar se as imagens têm o mesmo número de linhas
    if (image1.rows != image2.rows) {
        std::cerr << "As imagens devem ter o mesmo número de linhas para concatenar horizontalmente." << std::endl;
        return -1;
    }

    // Criar uma nova imagem concatenando as duas imagens horizontalmente
    cv::Mat concatImage;
    cv::hconcat(image1, image2, concatImage);

    // Exibir a imagem concatenada
    cv::imshow("Imagens Lado a Lado", concatImage);

    // Aguardar por uma tecla
    cv::waitKey(0);

    return 0;
}
