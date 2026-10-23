#include "httplib.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using namespace httplib;

const char* PAGINA = R"HTML(
<!DOCTYPE html>
<html>
<body>
  <h2>Teste OpenCV + C++</h2>
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

int main() {
    Server srv;

    /*
    srv.Get("/", [](const Request&, Response& res) {
        res.set_content(PAGINA, "text/html");
    });

    srv.Post("/processar", [](const Request& req, Response& res) {
        vector<uchar> dados(req.body.begin(), req.body.end());
        Mat img = imdecode(dados, IMREAD_COLOR);

        if (img.empty()) {
            res.status = 400;
            res.set_content("Imagem invalida", "text/plain");
            return;
        }

    });
    */
    Mat img = imread("IMG_20230504_090659.bmp", IMREAD_COLOR);
   
    if (img.empty()) {
        cout << "erro ao carregar a imagem" << endl;
        return -1;
    } 

    // cout << img << endl;

    //cout << "Servidor rodando em http://localhost:8080" << endl;
    //srv.listen("0.0.0.0", 8080);
}