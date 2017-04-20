$(document).ready(function(){
    alert("good");
    $("#login").click(function(){
        var user = $("username").val();
        var pwd = $("#password").val();
        alert("Username: "+user);
    });
});