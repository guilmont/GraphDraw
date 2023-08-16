#include <random>
#include <glm/gtx/vector_angle.hpp>

#include <GRender/utils.h>

#include "graphDraw.hpp"



std::ostream& operator<<(std::ostream& out, const glm::vec2& var) {
    out << var.x << ", " << var.y;
    return out;
}

std::ostream& operator<<(std::ostream& out, const glm::vec3& var) {
    out << var.x << ", " << var.y << ", " << var.z;
    return out;
}


GraphDraw::GraphDraw() : Application("Graph drawing", 1200, 800, "assets/layout.ini") {
    m_View = GRender::Viewport({ 1200, 800 });
    m_Camera = GRender::Camera({ 0.0f, 0.0f, 25.0f });
    m_Camera.sensitivity() = 20.0f;
    m_Camera.open();

    // Generating a random graph for testing algorihtms
    generateRandomGraph(50);
}

void GraphDraw::onUserUpdate(float deltaTime) {
    
    auto seed = std::random_device()();
    std::default_random_engine ran(seed);
    std::normal_distribution<float> normal(0.0f, 1.0f);

    const size_t numNodes = m_Nodes.size();
    const size_t maxSteps = 10;

    const float dTime = 0.001f;

    for (size_t step = 0; step < maxSteps; ++step) {
        std::vector<glm::vec2> vAttractive(numNodes, { 0.0f, 0.0f });
        std::vector<glm::vec2> vRepulsion(numNodes, { 0.0f, 0.0f });

        for (const Node& nd : m_Nodes) {
            const size_t nodeId = nd.index();

            for (const Node* ed : nd.edges) {
                // Calculating attractive forces
                const float attCoeff = glm::distance2(ed->position, nd.position) / m_IdealDistance;
                const glm::vec2 dir = glm::normalize(ed->position - nd.position);

                vAttractive.at(nodeId) +=  attCoeff * dir;
                vAttractive.at(ed->index()) -= attCoeff * dir;
            }

            for (Node& another : m_Nodes) {
                // Calculating repulstion forces
                if (nodeId == another.index()) { continue; }
                
                const float repCoeff = m_IdealDistance * m_IdealDistance / glm::distance(another.position, nd.position);
                const glm::vec2 dir = glm::normalize(another.position - nd.position);

                vRepulsion.at(nodeId) -=  repCoeff * dir;
                vRepulsion.at(another.index()) +=  repCoeff * dir;
            }
        }

        // Updating position
        for (Node& nd : m_Nodes) {
            const size_t nodeId = nd.index();
            const glm::vec2 force = m_AttractionCoefficient * vAttractive.at(nodeId)
                                  + m_RepulsionCoefficient * vRepulsion.at(nodeId);
            nd.position += force * dTime + sqrtf(2.0f * m_NoiseStrength * dTime) * glm::vec2{normal(ran), normal(ran)};
        }
    }

    // Moving center of mass to zero
    glm::vec2 centerOfMass = { 0.0f, 0.0f };
    for (Node& nd : m_Nodes) {
        centerOfMass += nd.position;
    }
    centerOfMass /= static_cast<float>(numNodes);
    for (Node& nd : m_Nodes) {
        nd.position -= centerOfMass;
    }

    /// VISUALIZATION /////////////////////////////////////
    if (m_View.hovered()) {
        m_Camera.controls(deltaTime);
    }
    
    m_View.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    GRender::quad::Specification spec;
    for (const Node& nd : m_Nodes) {
        for (const Node* ed : nd.edges) {
            spec.position = glm::vec3{ 0.5f * (nd.position + ed->position), 0.0f };
            spec.size = { glm::distance(nd.position, ed->position), 0.1f };

            const glm::vec2 direction = glm::normalize(ed->position - nd.position);
            spec.angle = glm::angle(glm::vec2{1.0f, 0.0}, direction);
            
            if (direction.y < 0.0f) {
                spec.angle = 2.0f * glm::pi<float>() - spec.angle;
            }

            m_Quad.submit(spec);
        }
    }
    spec = GRender::quad::Specification();
    for (const Node& nd : m_Nodes) {
        spec.position = glm::vec3{ nd.position, 0.0f };
        spec.color = glm::vec4{ nd.color, 1.0f };
        m_Quad.submit(spec);
    }
    m_Quad.draw(m_Camera.getViewMatrix());
    m_View.unbind();
}

void GraphDraw::ImGuiLayer(void) {
    GRender::utils::PerformanceDisplay(m_ViewStats);

    m_Camera.display();
    m_View.display("Viewport");

    // Update camera aspect ratio in case viewport changed
    glm::vec2 vsz = m_View.size();
    m_Camera.aspectRatio() = vsz.x / vsz.y;

    ImGui::Begin("Configuration");

    const float split = 0.5f;
    const float width = 0.95f;

    size_t numNodes = m_Nodes.size();
    GRender::utils::Drag("Number of Nodes:", numNodes, split, width);
    GRender::utils::Drag("Average edges count:", m_AvgEdgesCount, split, width);
    if (ImGui::Button("Regenerate")) {
        generateRandomGraph(numNodes);
    }

    ImGui::Separator();

    GRender::utils::Drag("Attraction coefficient:", m_AttractionCoefficient, split, width);
    GRender::utils::Drag("Repulsion coefficient:", m_RepulsionCoefficient, split, width);
    GRender::utils::Drag("Noise strength:", m_NoiseStrength, split, width);
    
    float distance = m_IdealDistance;
    if (GRender::utils::Drag("Ideal distance:", distance, split, width, 1.0)) {
        m_IdealDistance = distance > 0.0f ? distance : m_IdealDistance;
    }

    ImGui::End();
}

void GraphDraw::ImGuiMenuLayer(void) {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit")) { closeApp(); }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("About")) {
        if (ImGui::MenuItem("View mailbox")) { GRender::mailbox::Open(); }
        if (ImGui::MenuItem("View stats")) { m_ViewStats = true; }
        
        ImGui::EndMenu();
    }
}
 

void GraphDraw::generateRandomGraph(const size_t numNodes) {

    auto seed = std::random_device()();
    std::default_random_engine eng(seed);

    std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);
    std::uniform_real_distribution<float> randomRegion(-1.0f, 1.0f);
    std::exponential_distribution<float> randomNumEdges(1.0f / m_AvgEdgesCount);
    std::uniform_int_distribution<size_t> randomEdge(0, numNodes - 1);

    m_Nodes.clear();
    m_Nodes.reserve(numNodes);
    for (size_t k = 0; k < numNodes; ++k) {
        auto& nd = m_Nodes.emplace_back(k);
        nd.color = { randomColor(eng), randomColor(eng), randomColor(eng) };
        nd.position = glm::vec2{ randomRegion(eng), randomRegion(eng) };
    }

    for (Node& nd : m_Nodes) {
        // For the moment, I like to have at least one edge to every node
        const size_t numEdges = static_cast<size_t>(randomNumEdges(eng)) + 1;
        for (size_t k = 0; k < numEdges; ++k) {
            // We don't want nodes with edges into itself
            while (true) {
                Node* ed = m_Nodes.data() + randomEdge(eng);
                if (ed != &nd) {
                    nd.edges.push_back(ed);
                    break;
                }
            }
        }
    }

    // Generating buffer for all drawings
    size_t numQuads = numNodes;
    for (const Node& nd : m_Nodes) { numQuads += nd.edges.size(); }
    m_Quad = GRender::Quad(static_cast<int32_t>(numQuads));
}