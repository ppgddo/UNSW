<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>


<html>
<body>

	<c:forEach var="tempArtist" items="${artist_list}">
	
		${tempArtist} <br/>
	
	</c:forEach>

</body>
</html>
