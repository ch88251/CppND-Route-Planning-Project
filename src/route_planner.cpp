#include "route_planner.h"
#include <algorithm>

using std::cout;

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y) : m_Model(model) {
  // Convert inputs to percentage:
  start_x *= 0.01;
  start_y *= 0.01;
  end_x *= 0.01;
  end_y *= 0.01;

  start_node = &m_Model.FindClosestNode(start_x, start_y);
  end_node = &m_Model.FindClosestNode(end_x, end_y);
}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  current_node->FindNeighbors();
  for (auto neighbor : current_node->neighbors) {
    neighbor->parent = current_node;
    neighbor->h_value = CalculateHValue(neighbor);
    neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
    neighbor->visited = true;
    open_list.push_back(neighbor);
  }
}

bool Compare(const std::vector<int> a, const std::vector<int> b) {
  int f1 = a[2] + a[3]; // f1 = g1 + h1
  int f2 = b[2] + b[3]; // f2 = g2 + h2
  return f1 > f2;
}

RouteModel::Node *RoutePlanner::NextNode() {
  std::sort(open_list.begin(), open_list.end(), [](const auto &a, const auto &b)
            { return (a->h_value + a->g_value) < (b->h_value + b->g_value); });
  RouteModel::Node *lowest = open_list.front();
  open_list.erase(open_list.begin());
  return lowest;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
  distance = 0.0f;
  std::vector<RouteModel::Node> path_found;
  RouteModel::Node parent_node;

  while (current_node->parent != nullptr) {
    path_found.emplace(path_found.begin(), *current_node);
    parent_node = *(current_node->parent);
    distance += current_node->distance(parent_node);
    current_node = current_node->parent;
  }

  path_found.emplace(path_found.begin(), *current_node);
  distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
  return path_found;
}

// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {

  // TODO: Implement your solution here.
  start_node->visited = true;
  open_list.push_back(start_node);
  RouteModel::Node *current_node = nullptr;

  while (open_list.size()>0) {
      current_node = NextNode();

      if (current_node->distance(*end_node) == 0) {
          m_Model.path = ConstructFinalPath(current_node);
          return;
      }
      AddNeighbors(current_node);
  }  
}
