#include "httplib.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using namespace httplib;

const char *PAGINA = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Morfologia Matematica</title>
</head>
<body>
  <h2>Detecção de Fronteiras</h2>
  <input type="file" id="arquivo" accept="image/*">
  <button onclick="enviar()">Processar</button><br><br>
  <img id="resultado" style="max-width:600px">
  <script>
    async function enviar() {
      const f = document.getElementById('arquivo').files[0];
      if (!f) return;
      const resp = await fetch('/processar', { method: 'POST', body: f });
      const blob = await resp.blob();
      document.getElementById('resultado').src = URL.createObjectURL(blob);
    }
  </script>
</body>
</html>
)HTML";

Mat bin(Mat img){
  Mat img_bin(img.rows, img.cols, CV_8UC1);
  for (int i = 0; i < img.rows; i++){
    for (int j = 0; j < img.cols; j++){
      Vec3b pixel = img.at<Vec3b>(i, j);
      int media = (pixel[0] + pixel[1] + pixel[2]) / 3;

      if (media < 128){
        img_bin.at<uchar>(i, j) = 0;
      } else {
        img_bin.at<uchar>(i, j) = 255;
      }
    }
  }
  return img_bin;
}

Mat pad(Mat img_bin){
  Mat img_pad(img_bin.rows+2, img_bin.cols+2, CV_8UC1, Scalar(255));
  for (int i = 0; i < img_bin.rows; i++){
    for (int j = 0; j < img_bin.cols; j++){
      img_pad.at<uchar>(i+1, j+1) = img_bin.at<uchar>(i, j);
    }
  }
  return img_pad;
}

Mat erosao(Mat img_pad){
  Mat img_erosao(img_pad.rows, img_pad.cols, CV_8UC1);
  for (int i = 0; i < img_pad.rows; i++){
    for (int j = 0; j < img_pad.cols; j++){
      img_erosao.at<uchar>(i, j) = img_pad.at<uchar>(i, j);
    }
  }

  // Erosão
  for (int i = 1; i < img_pad.rows - 1; i++){
    for (int j = 1; j < img_pad.cols - 1; j++){
      // verifica se o pixel central é preto, ou seja, objeto
      bool flag = false;
      if (img_pad.at<uchar>(i, j) == 0){
        for (int k = -1; k <= 1 && flag == false; k++){
          for (int l = -1; l <= 1 && flag == false; l++){
            if (img_pad.at<uchar>(i + k, j + l) == 255){
              // se algum pixel vizinho for branco, o pixel central é definido como branco
              img_erosao.at<uchar>(i, j) = 255;
              flag = true;
            }
          }
        }
      }
    }
  }
  return img_erosao;
}

Mat borda(Mat img_erosao, Mat img_pad){
  Mat img_resultado(img_erosao.rows-2, img_erosao.cols-2, CV_8UC3, Scalar(255, 255, 255));
  // Destaca a borda do objeto
  for (int i = 1; i < img_erosao.rows - 1; i++){
    for (int j = 1; j < img_erosao.cols - 1; j++){
      if (img_erosao.at<uchar>(i, j) == 255 && img_pad.at<uchar>(i, j) == 0){
        img_resultado.at<Vec3b>(i-1, j-1) = Vec3b(0, 0, 255); // Destaca a borda do objeto em vermelho
      }
    }
  }
  return img_resultado;
}

int main()
{
  Server srv;

  srv.Get("/", [](const Request &, Response &res){ 
    res.set_content(PAGINA, "text/html"); 
  });

  srv.Post("/processar", [](const Request &req, Response &res){
    vector<uchar> dados(req.body.begin(), req.body.end());
    Mat img = imdecode(dados, IMREAD_COLOR);

    if (img.empty()){
      res.status = 400;
      res.set_content("Imagem invalida", "text/plain");
      return;
    }

    Mat img_bin = bin(img);
    Mat img_pad = pad(img_bin);
    Mat img_erosao = erosao(img_pad);
    Mat img_resultado = borda(img_erosao, img_pad);

    // Imagem processada
    vector<uchar> resultado;
    imencode(".bmp", img_resultado, resultado);
    res.set_content(string(resultado.begin(), resultado.end()), "image/bmp");

    // cout << img << endl;
  });

  cout << "Servidor rodando em http://localhost:8080" << endl;
  srv.listen("0.0.0.0", 8080);

}