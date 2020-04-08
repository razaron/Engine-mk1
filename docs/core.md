Core                         {#core}
=============

Manages the sending, receiving and processing of asynchronous events.

## Architecture


LOGIC                               | DATA
----------------------------------- | -----------------------------------------
\dotfile core_loop.dot "Core Loop"  | \dotfile core_data.dot "Core Data Layout"

## Examples

### Creating an Event

```{cpp}
int data = 42;
Event e {
    recipientID, // UUID64 for ID of recipient
    EventType::SPACE_NEW_ENTITY, // EventType enum for the type of Event  
    std::make_shared<int>(42) // std::shared_ptr<void> for data
}
```