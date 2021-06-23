#include <iostream>
#include <memory>

class BaseConn {
public:
    BaseConn() = default;

    ~BaseConn() = default;

public:
    virtual void onClose();
};

class TcpConn : public BaseConn {
public:
    void onClose() override;
};

void TcpConn::onClose() {
    BaseConn::onClose();
    std::cout << "TcpConn onClose" << std::endl;
}

void BaseConn::onClose() {
    std::cout << "BaseConn onClose" << std::endl;
}

int main() {
    std::shared_ptr<BaseConn> conn = std::make_shared<TcpConn>();
    conn->onClose();
    return 0;
}
