<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>


<html>
<body>
	<div>
		<h3>Search results songs songs</h3>
		
		<form action="ControllerServlet" method="GET">
		
			<input type="hidden" name="control_cmd" value="CART" />
			
			<input type="submit" value="Add to Chart" class="save" />
		
			<table border="1">
			
				<tr>
					<th>Title </th>
					<th>Artist </th>
					<th>Album </th>
				</tr>
				
				<c:forEach var="tempSong" items="${SONG_LIST}">
																		
					<tr>
					<td> ${tempSong.title} </td>
					<td> ${tempSong.artist} </td>
						<td> <input type="checkbox" name="add" value=${tempSong.id}/> </td>
					</tr>
				
				</c:forEach>
				
			</table>
		</form>
	</div>
	
	
	<p>
		<a href="ControllerServlet">Back to search</a>
	</p>
	
	
	
	

</body>
</html>
