package lt.avr.movingmessage.rss;

import java.io.Serializable;

import org.simpleframework.xml.Element;
import org.simpleframework.xml.Namespace;
import org.simpleframework.xml.NamespaceList;
import org.simpleframework.xml.Root;


//@NamespaceList({
//@Namespace,
//@Namespace(reference="http://www.w3.org/2005/Atom", prefix = "atom")
//})

@Root(name = "rss", strict = false)
public class Rss implements Serializable {
	private static final long serialVersionUID = 1539124340535129063L;
	
	@Element
	private Channel channel;

	public Channel getChannel() {
		return channel;
	}

	public void setChannel(Channel channel) {
		this.channel = channel;
	}
}
