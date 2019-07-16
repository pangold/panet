#### Basic & Common Classes

Implemented a serial of basic & common classes, such as:

buffer, byte/bit stream, logger, sequencer, endianness, thread/thread pool, queue/stream executor, etc..

#### Basic Communication Modules

1. **tcp module**: basic communication module(tcp only). 
2. **protocol module**: customized protocol `pan::net::protocol::datagram` and protobuf codec module.
3. **pattern module**: for common scenarios of interaction, such as: request/reply, publish/subscribe, push/pull(similar to ZeroMQ)

#### Others Modules

1. **rpc module**: based on request/reply model from **pattern module**.
2. **others**: had been obsoleted.

#### New features(comming soon)

1. Automatically reconnection for clients. Then each part of the system shouldn't be launched in a particular order.
2. Statistic.
