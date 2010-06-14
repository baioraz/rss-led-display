/**
 * 
 */
package lt.avr.movingmessage.junit;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.util.List;

import lt.avr.movingmessage.rss.Channel;
import lt.avr.movingmessage.rss.Item;
import lt.avr.movingmessage.rss.Rss;
import lt.avr.movingmessage.rss.RssReader;

import org.junit.Test;

/**
 * @author baioraz
 *
 */
public class RssReaderTest {

	/**
	 * Test method for {@link lt.avr.movingmessage.rss.RssReader#getChannel()}.
	 * @throws IOException 
	 * @throws UnsupportedEncodingException 
	 */
//	@Test
//	public void testGetChannel() throws UnsupportedEncodingException, IOException {
//		
//		RssReader rssReader = new RssReader("http://newsrss.bbc.co.uk/rss/newsonline_world_edition/front_page/rss.xml");
//		Rss rss = rssReader.getRss();
//		System.out.println("############################");
//		Channel channel = rss.getChannel();
//		System.out.println("Title - " + channel.getTitle());
//		System.out.println("Ttl - " + channel.getTtl());
//		List<Item> items = channel.getItems();
//		for (int i = 0; i < items.size(); i++) {
//			Item item = items.get(i);
//			System.out.println("ITEM");
//			System.out.println("Category: " + item.getCategory());
//			System.out.println("Description: " + item.getDescription());
//			System.out.println("Link: " + item.getLink());
//			System.out.println("PubDate: " + item.getPubDate());
//			System.out.println("Title: " + item.getTitle());
//			System.out.println("/ITEM");
//		}
//		
//		//fail("Not yet implemented");
//	}

}
