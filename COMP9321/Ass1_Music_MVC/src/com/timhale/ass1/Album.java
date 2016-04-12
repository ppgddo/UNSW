package com.timhale.ass1;

import java.util.List;

public class Album {

	private final String title;
	private final String artist;
	private final String genre;
	private final String publisher;
	private final String year;
	private final String id;
	private String price;
	private List<Song> songList;

	public Album(String title, String artist, String genre, String publisher, String year, String id) {
		super();
		this.title = title;
		this.artist = artist;
		this.genre = genre;
		this.publisher = publisher;
		this.year = year;
		this.id = id;
	}

	public String getTitle() {
		return title;
	}

	public String getArtist() {
		return artist;
	}

	public String getGenre() {
		return genre;
	}

	public String getPublisher() {
		return publisher;
	}

	public String getYear() {
		return year;
	}

	public String getId() {
		return id;
	}

	public String getPrice() {
		return price;
	}

	public void setPrice(String price) {
		this.price = price;
	}

	public List<Song> getSongList() {
		return songList;
	}

	public void setSongList(List<Song> songList) {
		this.songList = songList;
	}
	
	public void setSong(Song song) {
		this.songList.add(song);
	}




}
