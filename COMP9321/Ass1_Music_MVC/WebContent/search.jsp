<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>


<!DOCTYPE html>
<html>

<head>
	<title>Music Store</title>
</head>

<body>
	<img src="spinning-records.jpg"/>

	<div>
		<div>
			<h2>Welcome to Sir Bling's Music Store</h2>
		</div>
	</div>
	
	<div>
		<h3>Search</h3>
		
		<table>
			<tbody>
				<form action="ControllerServlet" method="GET">
				
					<input type="hidden" name="control_cmd" value="RESULTS" />

						<td><label>Search Text:</label></td>
						<td><input type="text" name="searchText" 
								   value="" /></td>
								   
						<td><select name="searchOption">
							    <option value="Anything">Anything</option>
							    <option value="Album">Album</option>
							    <option value="Artist">Artist</option>
							    <option value="Song">Songs</option>
						  	</select>
						</td>

						<td><label></label></td>
						<td><input type="submit" value="Search" class="save" /></td>
				</form>

				<form action="ControllerServlet" method="GET">
				
					<input type="hidden" name="control_cmd" value="CART" />
						<td><input type="submit" value="Go to Shopping Cart" class="save" /></td>
				</form>
			</tbody>
		</table>
	</div>

		
	<div>
		<h3>Random 10 songs</h3>
		<table border="1">
		
				<tr>
					<th>Title </th>
					<th>Artist </th>
					<th>Album </th>
					<th>Genre </th>
					<th>Publisher </th>
					<th>Year </th>
					<th>Price </th>
				</tr>
				
				<c:forEach var="tempSong" items="${SONG_LIST}">
																		
					<tr>
					<td> ${tempSong.title} </td>
					<td> ${tempSong.artist} </td>
					<td> ${tempSong.albumTitle} </td>
					<td> ${tempSong.genre} </td>
					<td> ${tempSong.publisher} </td>
					<td> ${tempSong.year} </td>
					<td> ${tempSong.price} </td>
					</tr>
				
				</c:forEach>
			
		</table>
	</div>
</body>

</html>











