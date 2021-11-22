# Relatório - Atividade 2 - Aplicação 3D #

Este repositório contém o código fonte e link da aplicação rodando em <br>
WebAssembly.

A aplicação foi implementada utilizando a seguinte arquitetura: C++, OpenGL e a biblioteca ABCG. <br>

__Membros:__

__Nome:__ Paulo Alexander Simões, <b>RA:</b>11084915 <br>
__Nome:__ Rodolfo Azevedo dos Santos, <b>RA:</b> 11093514

__Link para a aplicação:__ https://comp-ufabc-cg.github.io/Atividade2-Paulo-Rodolfo/public/   (caso encontre problemas ao abrir, é necessário tentar outras vezes recarregando a página)

<br>__Aplicação:__ Mustang 3D

<br> <b>Teoria: </b>

<ul>
 <li>O modelo 3D desenvolvido com a biblioteca ABCG utiliza conceitos de álgebra linear para a construção e renderização do objeto.</li>
 <li>A orientação da superfície é determinada utilizando a pipeline do OpenGL.</li>
 <li>A forma do nosso modelo é composta por n triângulos formados pelos vertíces (x,y,z) que unidos formam a imagem 3D do mustang.</li>
 <li>Face culling: Para otimizar a renderização, habilitamos a seleção do face culling através do checkbox. Caso habilitado, o face culling é ativado a partir da função abcg::glEnable(GL_CULL_FACE). O Face Culling consiste em descartar todos os triângulos que não estão de frente para a câmera.</li>
 <li>A orientação dos vértices é no sentido horário. Habilitado através da função abcg::glFrontFace(GL_CW)</li>
</ul>




<br>__Funcionalidades:__

1) Slider para controle de rotação:
- Permite o controle da velocidade no sentido horário (velocidade positiva) e anti horário (velocidade negativa). A velocidade é medida em radianos. <br>
![image](https://user-images.githubusercontent.com/30665585/142787184-b83cad7e-5c3c-48eb-a8ac-f6963c8fab56.png)

2) Quantidade de triangulos para renderizar a imagem: <br>
- Permite selecionar a quantidade de triângulos da aplicação 3D.
![image](https://user-images.githubusercontent.com/30665585/142787121-5ff6bb34-c265-4a4d-8aca-833ec97ff869.png)

3) Seleção de cores do carro:
- Permite a seleção de cores do carro, vide abaixo. Os vértices são coloridos a partir da cor selecionada. <br>
![image](https://user-images.githubusercontent.com/30665585/142787067-ba6e9555-80a4-46cb-a6bb-17e920b96728.png)



4) Face culling
- Checkbox para habilitar o face culling.  <br>
![image](https://user-images.githubusercontent.com/30665585/142787253-d7e35f95-3f48-4246-80fd-2d1e7a6c1b2a.png)



<br>__Implementação:__<br>
Para carregar o Mustang (carro) de nossa aplicação 3D, utilizamos a função "loadModelFromFile". O modelo 3d foi obtido no site Cgtrader (https://www.cgtrader.com/items/2729539/download-page).
A edição do arquivo e transformação em obj foi realizada no MeshLab.

O slider para controle de rotação é implementado a partir do ImGui. O intervalo varia de -500 a +500 radianos (intervalo contínuo, ou seja, tipo float). Ao selecionar uma velocidade positiva o carro rotaciona para a direita (sentido horário), caso contrário, rotaciona para a esquerda (sentido anti-horário).

O slider para controle de triangulos também é implementado a partir do ImGui. O intervalo aceita números inteiros.

O combobox criado para a alteração de cores é implentando a partir da lib ImGui. As cores foram pré definidas em um vetor de cores RGB para seleção do usuário. Foi criada uma variável de controle uniforme para acompanhar a alteração de todos os vertíces quando selecionado pelo usuário
