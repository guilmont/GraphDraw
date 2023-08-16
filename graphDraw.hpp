#include <vector>

#include "GRender/application.h"

#include "GRender/camera.h"
#include "GRender/quad.h"
#include "GRender/shader.h"
#include "GRender/viewport.h"

class Node {
public:
    Node(size_t index) : m_Index(index) {}

    size_t index() const { return m_Index; }

    glm::vec3 color;
    glm::vec2 position;
    std::vector<Node*> edges;

private:
    size_t m_Index;
};

class GraphDraw : public GRender::Application {
public:
    GraphDraw(void);
    ~GraphDraw(void) = default;

    void onUserUpdate(float deltaTime) override;
    void ImGuiLayer(void) override;
    void ImGuiMenuLayer(void) override;

    void generateRandomGraph(const size_t numNodes);

private:
    bool m_ViewStats = false;
    GRender::Viewport m_View;
    GRender::Camera m_Camera;
    GRender::Quad m_Quad;

private:
    float m_AvgEdgesCount = 1.0f / 3.0f;

    float m_IdealDistance = 1.0f;
    float m_AttractionCoefficient = 1.0f;
    float m_RepulsionCoefficient = 2.0f;

    float m_NoiseStrength = 0.5f;

    std::vector<Node> m_Nodes;
};
