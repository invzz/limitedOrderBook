#include "order.hh"
#include <cassert>

// Constructor
Order::Order(int id, OrderType type, double price, int quantity, int userId)
    : id(id), type(type), price(price), quantity(quantity), userId(userId)
{}

// Getters
int Order::getId() const { return id; }

OrderType Order::getType() const { return type; }

double Order::getPrice() const { return price; }

int Order::getQuantity() const { return quantity; }

// Setters
void Order::setId(int newId) { id = newId; }

void Order::setType(OrderType newType) { type = newType; }

void Order::setPrice(double newPrice) { price = newPrice; }

void Order::setQuantity(int newQuantity) { quantity = newQuantity; }

// Update quantity by a specified delta
void Order::updateQuantity(int delta)
{
  quantity += delta;
  // Quantity should not go below zero
  if(quantity < 0)
    {
      quantity = 0; // Prevent negative quantities
    }
}