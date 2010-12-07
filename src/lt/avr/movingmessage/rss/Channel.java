package lt.avr.movingmessage.rss;

import java.io.Serializable;
import java.util.List;

import org.simpleframework.xml.Element;
import org.simpleframework.xml.ElementList;
import org.simpleframework.xml.Namespace;
import org.simpleframework.xml.Root;

@Root(name = "channel", strict = false)
public class Channel implements Serializable {
	private static final long serialVersionUID = -3191851495714280255L;

	@Element
	private String title;
	
//	@Element
//	private String link;
	
	//@Namespace(reference="http://www.w3.org/2005/Atom")
//	@ElementList(entry = "link", name="link", empty = true, required = false)
//	private String link;
	
	@Element
	private String description;
	
	@ElementList (name = "item", inline = true)
	private List<Item> items;

	public String getTitle() {
		return title;
	}

	public String getDescription() {
		return description;
	}

	public List<Item> getItems() {
		return items;
	}

//	@Element (name = "language", required = false)
//	private String language;
//	
//	@Element (name = "copyright", required = false)
//	private String copyright;
//	
//	@Element (name = "managingEditor", required = false)
//	private String managingEditor;
//	
//	@Element (name = "webMaster", required = false)
//	private String webMaster;
//	
//	@Element (name = "pubDate", required = false)
//	private String pubDate;
//	
//	@Element (name = "lastBuildDate", required = false)
//	private String lastBuildDate;
//	
//	@Element (name = "category", required = false)
//	private String category;
//	
//	@Element (name = "generator", required = false)
//	private String generator;
//	
//	@Element (name = "docs", required = false)
//	private String docs;
	
//	@Element (name = "cloud", required = false)
//	private Cloud cloud;
	
//	@Element (name = "ttl", required = false)
//	private String ttl;
	
//	@Element (name = "image", required = false)
//	private Image image;
	
//	@Element (name = "rating", required = false)
//	private String rating;
//	
//	@Element (name = "textInput", required = false)
//	private String textInput;
//	
//	@Element (name = "skipHours", required = false)
//	private String skipHours;
//	
//	@Element (name = "skipDays", required = false)
//	private String skipDays;
	
}
