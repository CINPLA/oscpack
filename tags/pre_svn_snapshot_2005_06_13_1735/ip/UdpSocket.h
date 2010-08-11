#ifndef INCLUDED_UDPSOCKET_H
#define INCLUDED_UDPSOCKET_H

#ifndef INCLUDED_NETWORKINGUTILITIES_H
#include "NetworkingUtils.h"
#endif /* INCLUDED_NETWORKINGUTILITIES_H */

#ifndef INCLUDED_IPENDPOINTNAME_H
#include "IpEndpointName.h"
#endif /* INCLUDED_IPENDPOINTNAME_H */


class PacketListener;
class TimerListener;

class UdpSocket;

class SocketReceiveMultiplexer{
    class Implementation;
    Implementation *impl_;

	friend class UdpSocket;

public:
    SocketReceiveMultiplexer();
    ~SocketReceiveMultiplexer();

	// only call the attach/detach methods _before_ calling Run

    // only one listener per socket, each socket at most once
    void AttachSocketListener( UdpSocket *socket, PacketListener *listener );
    void DetachSocketListener( UdpSocket *socket, PacketListener *listener );

    void AttachPeriodicTimerListener( int periodMilliseconds, TimerListener *listener );
	void AttachPeriodicTimerListener(
            int initialDelayMilliseconds, int periodMilliseconds, TimerListener *listener );
    void DetachPeriodicTimerListener( TimerListener *listener );  

    void Run();      // loop and block processing messages indefinitely
	void RunUntilSigInt();
    void Break();    // call this from a listener to exit once the listener returns
    void AsynchronousBreak(); // call this from another thread or signal handler to exit the Run() state
};


class UdpSocket{
    class Implementation;
    Implementation *impl_;
    
	friend class SocketReceiveMultiplexer::Implementation;
    
public:

	// ctor throws std::runtime_error if there's a problem
	// initializing the socket.
	UdpSocket();
	virtual ~UdpSocket();

	// the socket is created in an unbound, unconnected state
	// such a socket can only be used to send to an arbitrary
	// address using SendTo(). To use Send() you need to first
	// connect to a remote endpoint using Connect(). To use
	// ReceiveFrom you need to first bind to a local endpoint
	// using Bind().

	// retrieve the local endpoint name when sending to 'to'
    IpEndpointName LocalEndpointFor( const IpEndpointName& remoteEndpoint ) const;

	// Connect to a remote endpoint which is used as the target
	// for calls to Send()
	void Connect( const IpEndpointName& remoteEndpoint );	
	void Send( const char *data, int size );
    void SendTo( const IpEndpointName& remoteEndpoint, const char *data, int size );


	// Bind a local endpoint to receive incoming data. Endpoint
	// can be 'any' for the system to choose an endpoint
	void Bind( const IpEndpointName& localEndpoint );
	bool IsBound() const;

	int ReceiveFrom( IpEndpointName& remoteEndpoint, char *data, int size );
};


// convenience classes for transmitting and receiving
// they just call Connect and/or Bind in the ctor.
// note that you can still use a receive socket
// for transmitting etc

class UdpTransmitSocket : public UdpSocket{
public:
	UdpTransmitSocket( const IpEndpointName& remoteEndpoint )
		{ Connect( remoteEndpoint ); }
};


class UdpReceiveSocket : public UdpSocket{
public:
	UdpReceiveSocket( const IpEndpointName& localEndpoint )
		{ Bind( localEndpoint ); }
};


#endif /* INCLUDED_UDPSOCKET_H */
