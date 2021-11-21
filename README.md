# Relatório - Atividade 2 - Aplicação 3D #

Este repositório contém o código fonte e link da aplicação rodando em <br>
WebAssembly.

A aplicação foi implementada utilizando a seguinte arquitetura: C++, OpenGL e a biblioteca ABCG. <br>

__Membros:__

__Nome:__ Paulo Alexander Simões, <b>RA:</b>11084915 <br>
__Nome:__ Rodolfo Azevedo dos Santos, <b>RA:</b> 11093514

__Link para a aplicação:__ https://comp-ufabc-cg.github.io/Atividade2-Paulo-Rodolfo/public/

<br>__Aplicação:__ Mustang 3D

<br> <b>Teoria: </b>

<ul>
 <li>O modelo 3D desenvolvido com a biblioteca ABCG utiliza conceitos de álgebra linear para a construção e renderização do objeto.</li>
 <li>A orientação da superfície é determinada utilizando a pipeline do OpenGL.</li>
 <li>A forma do nosso modelo é composta por n triângulos formados pelos vertíces (x,y,z) que unidos formam a imagem 3D do mustang.</li>
 <li>Face culling: Para otimizar a renderização, habilitamos o face culling através da função abcg::glEnable(GL_CULL_FACE). O Face Culling consiste em descartar todos os triângulos que não estão de frente para a câmera.</li>
 <li>A orientação dos vértices é no sentido horário. Habilitado através da função abcg::glFrontFace(GL_CW)</li>
</ul>




<br>__Funcionalidades:__

1) Slider para controle de rotação:
- Permite o controle da velocidade no sentido horário (velocidade positiva) e anti horário (velocidade negativa). A velocidade é medida em radianos.

![image](https://user-images.githubusercontent.com/30665585/141701203-1e683108-a29b-4c94-82ca-1dcfffdc1bfb.png)

2) Quantidade de triangulos para renderizar a imagem: <br>
- Permite selecionar a quantidade de triângulos da aplicação 3D.
![image](https://user-images.githubusercontent.com/30665585/141701226-26846580-c62c-4aa2-bd80-13e9c3da758f.png)



<br>__Implementação:__<br>
Para carregar o Mustang (carro) de nossa aplicação 3D, utilizamos a função "loadModelFromFile". O modelo 3d foi obtido no site Cgtrader (https://www.cgtrader.com/items/2729539/download-page).
A edição do arquivo e transformação em obj foi realizada no MeshLab.

O slider para controle de rotação é implementado a partir do ImGui. O intervalo varia de -500 a +500 radianos (intervalo contínuo, ou seja, tipo float). Ao selecionar uma velocidade positiva o carro rotaciona para a direita (sentido horário), caso contrário, rotaciona para a esquerda (sentido anti-horário).

O slider para controle de triangulos também é implementado a partir do ImGui. O intervalo aceita números inteiros.
