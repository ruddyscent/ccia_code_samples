#include <future>
#include <vector>
#include <string>

class data_packet {
public:
    int id;
    std::string payload;
};

class outgoing_packet {
public:
    std::string payload;
    std::promise<bool> promise;
};

class connection {
public:
    bool has_incoming_data() const {
        // Placeholder implementation
        return true;
    }

    data_packet incoming() {
        // Placeholder implementation
        return data_packet{1, "data"};
    }

    std::promise<std::string>& get_promise(int id) {
        // Placeholder implementation
        static std::promise<std::string> p;
        return p;
    }

    bool has_outgoing_data() const {
        // Placeholder implementation
        return true;
    }

    outgoing_packet top_of_outgoing_queue() {
        // Placeholder implementation
        return outgoing_packet{"data", std::promise<bool>()};
    }

    void send(const std::string& payload) {
        // Placeholder implementation
    }
};

class connection_set {
public:
    using iterator = std::vector<connection>::iterator;

    iterator begin() {
        // Placeholder implementation
        return connections.begin();
    }

    iterator end() {
        // Placeholder implementation
        return connections.end();
    }

private:
    std::vector<connection> connections;
};

bool done(const connection_set& connections) {
    // Placeholder implementation
    return false;
}

void process_connections(connection_set& connections) {
    while(!done(connections)) {
        for(auto connection = connections.begin(), end = connections.end();
            connection != end;
            ++connection)
        {
            if(connection->has_incoming_data()) {
                data_packet data = connection->incoming();
                std::promise<std::string>& p = connection->get_promise(data.id);
                p.set_value(data.payload);
            }
            if(connection->has_outgoing_data()) {
                outgoing_packet data = connection->top_of_outgoing_queue();
                connection->send(data.payload);
                data.promise.set_value(true);
            }
        }
    }
}

int main() {
    connection_set connections;
    process_connections(connections);
    return 0;
}