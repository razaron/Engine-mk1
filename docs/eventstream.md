EventStream                         {#eventstream}
=============
Manages the sending, receiving and processing of asynchronous events.

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

### Pushing Events onto the EventStream

```{cpp}
EventStream stream;
// Pushing a single event
Event event{};
stream.push(event, StreamType::INCOMING);
std::vector<Event> events
stream.pushEvents(events, StreamType::INCOMING)
```

### Popping Events from the EventStream

```{cpp}
EventStream stream;
// ... Events get pushed on
// Pushing a single event
Event event = stream.pop(StreamType::INCOMING);
std::vector<Event> events = stream.popEvents(StreamType::INCOMING)
```

### Processing Events

Events in an EventStreams are processed by being passed though an appropriate handler for their event type.


```{cpp}
const EventType MY_EVENT_TYPE{"MyEvent"}

struct MyEventData
{
    std::string text;
};

EventStream stream;
EventHandler handler = [](const Event &e){
    MyEventData data = std::static_ptr_cast<MyEventData>(e.data);
    std::cout << data->text << std::endl;
};

stream.registerHandler(MY_EVENT_TYPE, handler);

// ...

stream.processEvents();
```