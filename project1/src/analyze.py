from graphviz import Digraph

# Function to parse tree text and generate the Graphviz visualization
def parse_tree_to_graph(tree_text, output_filename='decision_tree'):
    graph = Digraph(comment='Decision Tree', format='pdf')
    lines = tree_text.splitlines()
    
    # Stack to track parent nodes for nesting
    stack = []
    node_counter = 0
    
    for line in lines:
        # Remove leading spaces to determine the indentation level
        stripped_line = line.lstrip()
        indent_level = len(line) - len(stripped_line)
        
        # Create a unique node ID
        current_node = f"node{node_counter}"
        node_counter += 1
        
        if "Leaf:" in stripped_line:
            # Leaf node
            label = stripped_line.replace("Leaf: ", "")
            graph.node(current_node, label, shape="box")
        else:
            # Decision node
            label = stripped_line.replace("Node: ", "").strip()
            graph.node(current_node, label)
        
        if stack:
            # Determine the connection direction (left or right)
            parent_node, parent_indent = stack[-1]
            if indent_level > parent_indent:
                # Child of the last node in the stack
                graph.edge(parent_node, current_node)
            else:
                # Pop stack until we find the correct parent level
                while stack and stack[-1][1] >= indent_level:
                    stack.pop()
                if stack:
                    parent_node, _ = stack[-1]
                    graph.edge(parent_node, current_node)
        
        # Push current node onto the stack
        stack.append((current_node, indent_level))

    # Render the graph to a file
    graph.render(output_filename, view=True)

# Read the decision tree from file
with open('tree.txt', 'r') as file:
    tree_text = file.read()

# Generate the decision tree graph
parse_tree_to_graph(tree_text, output_filename='decision_tree')
