package lt.avr.movingmessage.rss;

import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URL;
import java.net.URLConnection;
import java.util.List;

import org.simpleframework.xml.Serializer;
import org.simpleframework.xml.core.Persister;

public class RssReader {

	private static String RSS_URL = null;
	private int currentEntry = 0;
	private int totalEntries = 0;
	private volatile static RssReader instance = null;

	public static RssReader getInstance(String rssUrl) {
		if (instance == null) {
			synchronized (RssReader.class) {
				if (instance == null) {
					instance = new RssReader(rssUrl);
				}
			}
		}
		return instance;
	}

	private RssReader(String rssUrl) {
		RSS_URL = rssUrl;
	}

	public String getNextEntry() throws IOException {
		String entry = getRssEntry(currentEntry);
		if (currentEntry < totalEntries - 1) {
			currentEntry++;
		} else {
			currentEntry = 0;
		}
		return entry;
	}

	public String getRssEntry(int index) throws IOException {
		StringBuffer data = new StringBuffer();
		Rss rss = getRss();
		Channel channel = rss.getChannel();
		List<Item> items = channel.getItems();
		totalEntries = items.size();
		if (index <= items.size()) {
			Item item = items.get(index);
			data.append(" " + item.getTitle() + " #:  " + item.getDescription() + "                                                          ");
			removeLithuanianSymbols(data);
		} else {
			data.append(" Rss entry not found ");
		}

		return data.toString();
	}

	private static Rss getRss() throws UnsupportedEncodingException, IOException {
		Rss rss = null;
		Serializer serializer = new Persister();
		InputStream inputStream = null;

		try {
			URL url = new URI(RSS_URL).toURL();
			URLConnection connection = url.openConnection();
			inputStream = connection.getInputStream();
			rss = serializer.read(Rss.class, inputStream);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (inputStream != null) {
				inputStream.close();
			}
		}
		return rss;
	}

	private static void removeLithuanianSymbols(StringBuffer rssData) {
		for (int i = 0; i < rssData.length(); i++) {

			char symbol = rssData.charAt(i);
			switch (symbol) {
			case 'à':
				rssData.setCharAt(i, 'a');
				break;
			case 'è':
				rssData.setCharAt(i, 'c');
				break;
			case 'æ':
				rssData.setCharAt(i, 'e');
				break;
			case 'ë':
				rssData.setCharAt(i, 'e');
				break;
			case 'á':
				rssData.setCharAt(i, 'i');
				break;
			case 'ð':
				rssData.setCharAt(i, 's');
				break;
			case 'ø':
				rssData.setCharAt(i, 'u');
				break;
			case 'û':
				rssData.setCharAt(i, 'u');
				break;
			case 'þ':
				rssData.setCharAt(i, 'z');
				break;
			case 'À':
				rssData.setCharAt(i, 'A');
				break;
			case 'È':
				rssData.setCharAt(i, 'C');
				break;
			case 'Æ':
				rssData.setCharAt(i, 'E');
				break;
			case 'Ë':
				rssData.setCharAt(i, 'E');
				break;
			case 'Á':
				rssData.setCharAt(i, 'I');
				break;
			case 'Ð':
				rssData.setCharAt(i, 'S');
				break;
			case 'Ø':
				rssData.setCharAt(i, 'U');
				break;
			case 'Û':
				rssData.setCharAt(i, 'U');
				break;
			case 'Þ':
				rssData.setCharAt(i, 'Z');
				break;
			case '„':
				rssData.setCharAt(i, '"');
				break;
			case '”':
				rssData.setCharAt(i, '"');
				break;
			case '“':
				rssData.setCharAt(i, '"');
				break;
			case '–':
				rssData.setCharAt(i, '-');
				break;
			default:
				break;
			}

		}
	}

}
