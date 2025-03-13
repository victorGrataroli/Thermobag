import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

temperatura = 38

def enviar_email(temperatura):
    remetente = "phpalyer@gmail.com"  
    senha = "ktoa nshl wzcq vlcn" 
    destinatario = "malsinis@sga.pucminas.br"
    assunto = "Temperatura"

    
    mensagem = MIMEMultipart()
    mensagem['From'] = remetente
    mensagem['To'] = destinatario
    mensagem['Subject'] = assunto

   
    mensagem_corpo = f"A temperatura da bolsa térmica ultrapassou {temperatura} graus."
    mensagem.attach(MIMEText(mensagem_corpo, 'plain'))

    try:
        # Conectar ao servidor SMTP (este exemplo usa o Gmail)
        servidor = smtplib.SMTP('smtp.gmail.com', 587)
        servidor.starttls()  # Ativar criptografia TLS
        servidor.login(remetente, senha)  # Login com o seu e-mail e senha

        # Enviar o e-mail
        servidor.sendmail(remetente, destinatario, mensagem.as_string())
        servidor.quit()  # Fechar a conexão com o servidor
        print("E-mail enviado com sucesso!")

    except Exception as e:
        print(f"Erro ao enviar o e-mail: {e}")

# Chamada da função
enviar_email(temperatura)
