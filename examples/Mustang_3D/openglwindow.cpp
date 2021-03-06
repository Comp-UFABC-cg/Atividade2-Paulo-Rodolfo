#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};
}  // namespace std

void OpenGLWindow::initializeGL() {
  glClearColor(0.3, 0.3, 0.3, 1);

  colors = {
    {0.402f, 1.000f, 0.404f, 1.0f}, // Green        (body)
    {0.578f, 0.516f, 0.418f, 1.0f}, // Brown        (coat)
    {0.671f, 0.659f, 0.522f, 1.0f}, // Light Brown  (scarf)
    {0.0f, 0.0f, 0.0f, 1.0f},       // Black        (eye)
    {1.0f, 1.0f, 1.0f, 1.0f},       // White
    {1.0f, 0.0f, 0.0f, 1.0f},       // Red
    {0.2f, 0.2f, 1.0f, 1.0f},       // Blue
    {0.2f, 0.7f, 1.0f, 1.0f},       // Light Blue
    {0.4f, 0.4f, 0.4f, 1.0f}        // Gray
  };

  backGroundColorIndex = 5;
  backGroundColor = colors[backGroundColorIndex];

  bodyWorkColorIndex = 1;
  bodyWorkColor = colors[bodyWorkColorIndex];

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "loadmodel.vert",
                                    getAssetsPath() + "loadmodel.frag");

  // Load model
  loadModelFromFile(getAssetsPath() + "mustang.obj");
  standardize();

  m_verticesToDraw = m_indices.size();

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const std::size_t startIndex{static_cast<size_t>(3 * index.vertex_index)};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void OpenGLWindow::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void OpenGLWindow::paintGL() {
  // Animate angle by 15 degrees per second
  const float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(1.0f) * deltaTime);

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLint colorLoc{glGetUniformLocation(m_program, "color")};
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);
  glBindVertexArray(m_VAO);
  

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  // Update uniform variable
  const GLint angleLoc{abcg::glGetUniformLocation(m_program, "angle")};
  abcg::glUniform1f(angleLoc, m_angle);

  // Draw triangles

  glUniform4f(colorLoc, bodyWorkColor[0], bodyWorkColor[1], bodyWorkColor[2], bodyWorkColor[3]); // set current selected body color to frag
  abcg::glDrawElements(GL_TRIANGLES, m_verticesToDraw, GL_UNSIGNED_INT,nullptr);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

   // Create a window for the other widgets
  {
    auto widgetSize{ImVec2(182, 60)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 20, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);
  //COLORS
  std::vector<std::string> comboItems{"Green", "Brown", "Light Brown", "Black", 
                                          "White", "Red", "Blue", "Light Blue",
                                          "Gray"};
  //FACECULLING
    static bool faceCulling{};
    ImGui::Checkbox("Back-face culling", &faceCulling);

    if (faceCulling) {
      abcg::glEnable(GL_CULL_FACE);
    } else {
      abcg::glDisable(GL_CULL_FACE);
    }

  // Alocar linha no combo
  ImGui::PushItemWidth(70);
        if (ImGui::BeginCombo("Cor do carro", comboItems.at(bodyWorkColorIndex).c_str())) {
          for (int index : iter::range(comboItems.size())) {
            const bool isSelected{bodyWorkColorIndex == index};
            if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
              bodyWorkColorIndex = index;
              bodyWorkColor = colors[bodyWorkColorIndex];
              
            }            
            if (isSelected) {
              ImGui::SetItemDefaultFocus();
            } 
          }
          ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::PopItemWidth();
  ImGui::PushItemWidth(70);
    
    ImGui::End();
  }


  // Create window for slider
  {
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportHeight - 94));
    ImGui::SetNextWindowSize(ImVec2(m_viewportWidth - 10, -1));
    ImGui::Begin("Slider window", nullptr, ImGuiWindowFlags_NoDecoration);

    // Create a slider to control the number of rendered triangles
    {
      // Slider will fill the space of the window
      ImGui::PushItemWidth(m_viewportWidth - 25);

      static int n{m_verticesToDraw / 3};
      ImGui::SliderInt("", &n, 0, m_indices.size() / 3, "%d triangles");
      m_verticesToDraw = n * 3;

      ImGui::PopItemWidth();
    }

    ImGui::End();
  }


  // Create window for slider
  {
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportHeight - 50));
    ImGui::SetNextWindowSize(ImVec2(m_viewportWidth - 5, -1));
    ImGui::Begin("Controle de Rota????o", nullptr, ImGuiWindowFlags_NoDecoration);

    // Controla a velocidade de rota????o
    {
      // Slider will fill the space of the window
      ImGui::PushItemWidth(m_viewportWidth - 25);

      static float n2;
      const float deltaTime2{static_cast<float>(getDeltaTime())};
      ImGui::SliderFloat("", &n2, -500.f, 500.f, "%.1f Radianos");
      m_angle = glm::wrapAngle(m_angle + glm::radians(n2) * deltaTime2);

      ImGui::PopItemWidth();
    }

    ImGui::End();
  }

      //abcg::glEnable(GL_CULL_FACE);
      abcg::glFrontFace(GL_CW); // Invertido
      //abcg::glFrontFace(GL_CCW); // Normal


  }
  


void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}